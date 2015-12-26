/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "SPUInstructionAnalyzer.h"

#include <QCoreApplication>

#include <boost/range/size.hpp>

#include <nc/common/CheckedCast.h>
#include <nc/common/Foreach.h>
#include <nc/common/Unreachable.h>
#include <nc/common/make_unique.h>

#include <nc/core/ir/Program.h>
#include <nc/core/irgen/Expressions.h>
#include <nc/core/irgen/InvalidInstructionException.h>

#include "SPUArchitecture.h"
#include "SPUInstruction.h"
#include "SPURegisters.h"

namespace nc {
namespace arch {
namespace spu {

namespace {

class SPUExpressionFactory: public core::irgen::expressions::ExpressionFactory<SPUExpressionFactory> {
public:
    SPUExpressionFactory(const core::arch::Architecture *architecture):
        core::irgen::expressions::ExpressionFactory<SPUExpressionFactory>(architecture)
    {}
};

typedef core::irgen::expressions::ExpressionFactoryCallback<SPUExpressionFactory> SPUExpressionFactoryCallback;

} // anonymous namespace

class SPUInstructionAnalyzerImpl {
    Q_DECLARE_TR_FUNCTIONS(SPUInstructionAnalyzerImpl)

    const SPUArchitecture *architecture_;
    SPUExpressionFactory factory_;
    core::ir::Program *program_;
    const SPUInstruction *instruction_;
    spu_instruction insn;

public:
    SPUInstructionAnalyzerImpl(const SPUArchitecture *architecture):
        architecture_(architecture), factory_(architecture)
    {}

    void createStatements(const SPUInstruction *instruction, core::ir::Program *program) {
        assert(instruction != nullptr);
        assert(program != nullptr);

        program_ = program;
        instruction_ = instruction;

        SPUExpressionFactoryCallback _(factory_, program->getBasicBlockForInstruction(instruction), instruction);

        createStatements(_, instruction, program);
    }

private:
    void createStatements(SPUExpressionFactoryCallback & _,
                                                       const SPUInstruction *instruction,
                                                       core::ir::Program *program) {
        disassemble(instruction);

        core::ir::BasicBlock *cachedDirectSuccessor = nullptr;
        auto directSuccessor = [&]() -> core::ir::BasicBlock * {
            if (!cachedDirectSuccessor) {
                cachedDirectSuccessor = program->createBasicBlock(instruction->endAddr());
            }
            return cachedDirectSuccessor;
        };

        using namespace core::irgen::expressions;

        auto operands = insn.operands;

        switch (insn.opcode) {
            /* Memory-Load/Store Instructions */
        case M_LQD:
            _[operand(0) ^= core::irgen::expressions::TermExpression(createDereferenceAddress(operands[1], 128))];
            break;
        case M_LQX:
            _[operand(0) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, SPUInstructionAnalyzer::createTerm(reg(operands[1].reg)), SPUInstructionAnalyzer::createTerm(reg(operands[2].reg)), 128), core::ir::MemoryDomain::MEMORY, 128))];
            break;
        case M_LQA:
        case M_LQR:
            _[operand(0) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::Constant>(SizedValue(128, operands[1].imm)), core::ir::MemoryDomain::MEMORY, 128))];
            break;
        case M_STQD:
            _[core::irgen::expressions::TermExpression(createDereferenceAddress(operands[1], 128)) ^= operand(0)];
            break;
        case M_STQX:
            _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, SPUInstructionAnalyzer::createTerm(reg(operands[1].reg)), SPUInstructionAnalyzer::createTerm(reg(operands[2].reg)), 128), core::ir::MemoryDomain::MEMORY, 128)) ^= operand(0)];
            break;
        case M_STQA:
        case M_STQR:
            _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::Constant>(SizedValue(128, operands[1].imm)), core::ir::MemoryDomain::MEMORY, 128)) ^= operand(0)];
            break;
            /* Constant-Formation Instructions */
        case M_ILH:
            _[operand(0) ^= operand(1)];
            break;
        case M_ILHU:
            _[operand(0) ^= operand(1) << constant(16)];
            break;
        case M_IL:
            _[operand(0) ^= operand(1)];
            break;
        case M_ILA:
            _[operand(0) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::Constant>(SizedValue(32, operands[1].imm)), core::ir::MemoryDomain::MEMORY, 128))];
            break;
        case M_IOHL:
            _[operand(0) ^= (operand(0) | zero_extend(operand(1,16)))];
            break;
        /*case M_FSMBI:
            break;*/
            /* Integer and Logical Instructions */
        case M_AH:
        case M_AHI:
        case M_A:
        case M_AI:
            _[operand(0) ^= (operand(1) + operand(2))];
            break;
        case M_SFH:
        case M_SFHI:
        case M_SF:
        case M_SFI:
            _[operand(0) ^= (operand(1) - operand(2))];
            break;
        case M_MPY:
        case M_MPYI: 
            _[operand(0) ^= (operand(1) * operand(2))];
            break;
        case M_MPYU:
        case M_MPYUI:
            _[operand(0) ^= (unsigned_(operand(1)) * unsigned_(operand(2)))];
            break;
        case M_MPYA:
            _[operand(0) ^= (operand(1) * operand(2)) + operand(3)];
            break;
        case M_XSBH:
            _[operand(0) ^= sign_extend(operand(1,8))];
            break;
        case M_XSHW:
            _[operand(0) ^= sign_extend(operand(1,16))];
            break;
        case M_XSWD:
            _[operand(0) ^= sign_extend(operand(1,32))];
            break;
        case M_AND:
        case M_ANDI:
        case M_ANDBI:
        case M_ANDHI:
            _[operand(0) ^= (operand(1) & operand(2))];
            break;
        case M_ANDC:
            _[operand(0) ^= (operand(1) & (~operand(2)))];
            break;
        case M_OR:
        case M_ORI:
        case M_ORBI:
        case M_ORHI: {
            if (operands[2].imm == 0) {
                _[operand(0) ^= operand(1)]; // M_LR
            } else {
                _[operand(0) ^= (operand(1) | operand(2))];
            }
            break;
        }
        case M_ORC:
            _[operand(0) ^= (operand(1) & (~operand(2)))];
            break;
        case M_XOR:
        case M_XORI:
        case M_XORBI:
        case M_XORHI:
            _[operand(0) ^= (operand(1) ^ operand(2))];
            break;
        case M_NAND:
            _[operand(0) ^= ~(operand(1) & operand(2))];
            break;
        case M_NOR:
            _[operand(0) ^= ~(operand(1) | operand(2))];
            break;
        case M_EQV:
            _[operand(0) ^= (operand(1) == operand(2))];
            break;
            /* Shift and Rotate Instructions */
        case M_SHLH:
        case M_SHLHI:
        case M_SHL:
        case M_SHLI:
        case M_SHLQBI:
        case M_SHLQBII:
            if (operands[2].imm == 0) {
                _[operand(0) ^= operand(1)];
            } else {
                _[operand(0) ^= (operand(1) << operand(2))];
            }
            break;
        case M_SHLQBY:
        case M_SHLQBYI:
            if (operands[2].imm == 0) {
                _[operand(0) ^= operand(1)];
            } else if (operands[2].imm > 15) {
                _[operand(0) ^= constant(0)];
            } else {
                _[operand(0) ^= (operand(1) << (operand(2) * constant(16)))];
            }
            break;
        case M_ROTH:
        case M_ROTHI:
        case M_ROT:
        case M_ROTI:
        case M_ROTQBI:
        case M_ROTQBII:
            _[operand(0) ^= (operand(1) << operand(2) | unsigned_(operand(1)) >> (constant(128) - operand(2)))];
            break;
        case M_ROTQBY:
        case M_ROTQBYI:
            if (operands[2].imm == 0) {
                _[operand(0) ^= operand(1)];
            } else {
                _[operand(0) ^= (operand(1) << (operand(2) * constant(16)) | unsigned_(operand(1)) >> (constant(128) - (operand(2) * constant(16))))];
            }
            break;
            /* Compare, Branch, and Halt Instruction */
        case M_HEQ:
        case M_HEQI:
        case M_HGT:
        case M_HGTI:
        case M_HLGT:
        case M_HLGTI:
            _(std::make_unique<core::ir::InlineAssembly>());
            break;
        case M_CEQB:
        case M_CEQBI:
        case M_CEQH:
        case M_CEQHI:
        case M_CEQ:
        case M_CEQI:
            _[operand(0) ^= sign_extend(operand(1) == operand(2))];
            break;
        case M_CGTB:
        case M_CGTBI:
        case M_CGTH:
        case M_CGTHI:
        case M_CGT:
        case M_CGTI:
            _[operand(0) ^= sign_extend(signed_(operand(1)) > operand(2))];
            break;
        case M_CLGTB:
        case M_CLGTBI:
        case M_CLGTH:
        case M_CLGTHI:
        case M_CLGT:
        case M_CLGTI:
            _[operand(0) ^= sign_extend(unsigned_(operand(1)) > operand(2))];
            break;
        case M_BR:
        case M_BRA:
        case M_BI:
            if (operands[0].reg == SPU_REG_LR) {
                _[jump(regizter(SPURegisters::r0()))];
            } else {
                _[jump(operand(0))];
            }
            break;
        case M_BRSL:
        case M_BRASL:
            if (operands[0].reg == SPU_REG_LR) {
                _[call(operand(1))];
            } else {
                _[jump(operand(1))];
            }
            break;
        case M_BRNZ:
        case M_BRHNZ:
        case M_BINZ:
        case M_BIHNZ:
            _[jump(~(operand(0) == constant(0)), operand(1), directSuccessor())];
            break;
        case M_BRZ:
        case M_BRHZ:
        case M_BIZ:
        case M_BIHZ:
            _[jump(operand(0) == constant(0), operand(1), directSuccessor())];
            break;
            /* Hint-for-Branch Instructions */
        case M_HBR:
        case M_HBRA:
        case M_HBRR:
            break;
            /* Floating-Point Instructions */
        case M_FA:
        case M_DFA:
        case M_FS:
        case M_DFS:
        case M_FM:
        case M_DFM:
            _(std::make_unique<core::ir::InlineAssembly>());
            break;
            /* Control Instructions */
        case M_STOP:
        case M_STOPD:
            _(std::make_unique<core::ir::InlineAssembly>());
            break;
        case M_NOP:
        case M_LNOP:
            break;
        case M_SYNC:
        case M_DSYNC:
            _(std::make_unique<core::ir::InlineAssembly>());
            break;
        case M_MFSPR:
        case M_MTSPR:
            _[operand(0) ^= operand(1)];
            break;
            /* Channel Instructions */
        case M_RDCH:
        case M_RCHCNT:
        case M_WRCH:
            _(std::make_unique<core::ir::InlineAssembly>());
            break;
        default:
            _(std::make_unique<core::ir::InlineAssembly>());
            break;
        }

    }

    void disassemble(const SPUInstruction *instruction) {
        auto opcode = *((int32_t *)instruction->bytes());
        spu_decode_insn(instruction->addr(), opcode, &insn);
    }

    core::irgen::expressions::TermExpression operand(std::size_t index, SmallBitSize sizeHint = 128) const {
        return core::irgen::expressions::TermExpression(createTermForOperand(index, sizeHint));
    }

    std::unique_ptr<core::ir::Term> createTermForOperand(std::size_t index, SmallBitSize sizeHint) const {
        auto &operand = insn.operands[index];
        switch (insn.operands[index].type) {
        case OP_INVALID:
            throw core::irgen::InvalidInstructionException(tr("The instruction does not have an argument with index %1").arg(index));
        case OP_REG: {
            return std::make_unique<core::ir::MemoryLocationAccess>(reg(operand.reg)->memoryLocation().resized(sizeHint));
            }
        case OP_IMM: {
            return std::make_unique<core::ir::Constant>(SizedValue(sizeHint, operand.imm));
            }
        case OP_MEM: {
            return std::make_unique<core::ir::Dereference>(createDereferenceAddress(operand, sizeHint), core::ir::MemoryDomain::MEMORY, sizeHint);
            }
        default:
            unreachable();
        }
    }

    std::unique_ptr<core::ir::Dereference> createDereferenceAddress(const spu_operand &operand, SmallBitSize sizeHint) const {
        if (operand.mem.disp) {
                return std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, SPUInstructionAnalyzer::createTerm(reg(operand.mem.base)), std::make_unique<core::ir::Constant>(SizedValue(128, operand.mem.disp)), 128), core::ir::MemoryDomain::MEMORY, sizeHint);
        } else {
                return std::make_unique<core::ir::Dereference>(SPUInstructionAnalyzer::createTerm(reg(operand.mem.base)), core::ir::MemoryDomain::MEMORY, sizeHint);
        }
    }

    static const core::arch::Register *reg(int reg) {
        switch (reg) {
#define REG(i) \
        case SPU_REG_R##i: return SPURegisters::r##i();
            REG(0) REG(1) REG(2) REG(3) REG(4) REG(5) REG(6) REG(7) REG(8) REG(9) 
            REG(10) REG(11) REG(12) REG(13) REG(14) REG(15) REG(16) REG(17) REG(18) REG(19)
            REG(20) REG(21) REG(22) REG(23) REG(24) REG(25) REG(26) REG(27) REG(28) REG(29)
            REG(30) REG(31) REG(32) REG(33) REG(34) REG(35) REG(36) REG(37) REG(38) REG(39) 
            REG(40) REG(41) REG(42) REG(43) REG(44) REG(45) REG(46) REG(47) REG(48) REG(49) 
            REG(50) REG(51) REG(52) REG(53) REG(54) REG(55) REG(56) REG(57) REG(58) REG(59) 
            REG(60) REG(61) REG(62) REG(63) REG(64) REG(65) REG(66) REG(67) REG(68) REG(69) 
            REG(70) REG(71) REG(72) REG(73) REG(74) REG(75) REG(76) REG(77) REG(78) REG(79) 
            REG(80) REG(81) REG(82) REG(83) REG(84) REG(85) REG(86) REG(87) REG(88) REG(89) 
            REG(90) REG(91) REG(92) REG(93) REG(94) REG(95) REG(96) REG(97) REG(98) REG(99) 
            REG(100) REG(101) REG(102) REG(103) REG(104) REG(105) REG(106) REG(107) REG(108) REG(109) 
            REG(110) REG(111) REG(112) REG(113) REG(114) REG(115) REG(116) REG(117) REG(118) REG(119)
            REG(120) REG(121) REG(122) REG(123) REG(124) REG(125) REG(126) REG(127)
            default:
                throw core::irgen::InvalidInstructionException(tr("Invalid register number: %1").arg(reg));
        }
    }
};

SPUInstructionAnalyzer::SPUInstructionAnalyzer(const SPUArchitecture *architecture):
    impl_(std::make_unique<SPUInstructionAnalyzerImpl>(architecture))
{}

SPUInstructionAnalyzer::~SPUInstructionAnalyzer() {}

void SPUInstructionAnalyzer::doCreateStatements(const core::arch::Instruction *instruction, core::ir::Program *program) {
    impl_->createStatements(checked_cast<const SPUInstruction *>(instruction), program);
}

}}} // namespace nc::arch::spu

/* vim:set et sts=4 sw=4: */
