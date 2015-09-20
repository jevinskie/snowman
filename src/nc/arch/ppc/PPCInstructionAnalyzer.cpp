/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "PPCInstructionAnalyzer.h"

#include <QCoreApplication>

#include <boost/range/size.hpp>

#include <nc/common/CheckedCast.h>
#include <nc/common/Foreach.h>
#include <nc/common/Unreachable.h>
#include <nc/common/make_unique.h>

#include <nc/core/arch/Capstone.h>
#include <nc/core/ir/Program.h>
#include <nc/core/irgen/Expressions.h>
#include <nc/core/irgen/InvalidInstructionException.h>

#include "PPCArchitecture.h"
#include "PPCInstruction.h"
#include "PPCRegisters.h"

namespace nc {
namespace arch {
namespace ppc {

namespace {

class PPCExpressionFactory: public core::irgen::expressions::ExpressionFactory<PPCExpressionFactory> {
public:
    PPCExpressionFactory(const core::arch::Architecture *architecture):
        core::irgen::expressions::ExpressionFactory<PPCExpressionFactory>(architecture)
    {}
};

typedef core::irgen::expressions::ExpressionFactoryCallback<PPCExpressionFactory> PPCExpressionFactoryCallback;

NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr0)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr0lt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr0gt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr0eq)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr1)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr1lt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr1gt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr1eq)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr2)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr2lt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr2gt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr2eq)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr3)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr3lt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr3gt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr3eq)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr4)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr4lt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr4gt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr4eq)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr5)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr5lt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr5gt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr5eq)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr6)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr6lt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr6gt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr6eq)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr7)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr7lt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr7gt)
NC_DEFINE_REGISTER_EXPRESSION(PPCRegisters, cr7eq)

} // anonymous namespace

class PPCInstructionAnalyzerImpl {
    Q_DECLARE_TR_FUNCTIONS(PPCInstructionAnalyzerImpl)

	const PPCArchitecture *architecture_;
    core::arch::Capstone capstone_;
    PPCExpressionFactory factory_;
    core::ir::Program *program_;
    const PPCInstruction *instruction_;
    core::arch::CapstoneInstructionPtr instr_;
    const cs_ppc *detail_;

public:
    PPCInstructionAnalyzerImpl(const PPCArchitecture *architecture):
        architecture_(architecture), capstone_(CS_ARCH_PPC, CS_MODE_64), factory_(architecture)
    {}

    void createStatements(const PPCInstruction *instruction, core::ir::Program *program) {
        assert(instruction != nullptr);
        assert(program != nullptr);

        program_ = program;
        instruction_ = instruction;
		
        PPCExpressionFactoryCallback _(factory_, program->getBasicBlockForInstruction(instruction), instruction);

        createStatements(_, instruction, program);
    }

private:
	void createStatements(PPCExpressionFactoryCallback & _,
                                                       const PPCInstruction *instruction,
                                                       core::ir::Program *program) {
		auto instr = disassemble(instruction);
		if (instr == nullptr)
			return;

		detail_ = &instr->detail->ppc;

		core::ir::BasicBlock *cachedDirectSuccessor = nullptr;
        auto directSuccessor = [&]() -> core::ir::BasicBlock * {
            if (!cachedDirectSuccessor) {
                cachedDirectSuccessor = program->createBasicBlock(instruction->endAddr());
            }
            return cachedDirectSuccessor;
        };

		using namespace core::irgen::expressions;

		switch (instr->id) {
		case PPC_INS_B: {
				switch (detail_->bc) {
					case PPC_BC_INVALID: // direct branch
						_[jump(operand(0))];
						break;
					case PPC_BC_LT:
							if (detail_->op_count > 1) {
								switch (detail_->operands[0].reg) {
								#define CREG(lowercase, uppercase) \
									case PPC_REG_##uppercase: { \
										_[ jump(lowercase##lt, operand(1), directSuccessor()) ]; \
									} break;
									CREG(cr2, CR2);
									CREG(cr3, CR3);
									CREG(cr4, CR4);
									CREG(cr5, CR5);
									CREG(cr6, CR6);
									CREG(cr7, CR7);
								}
							} else {
								_[ jump(cr0lt, operand(0), directSuccessor()) ];
							}
						break;
					case PPC_BC_LE: {
							PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
							if (detail_->op_count > 1) {
								then[jump(operand(1))];
								switch (detail_->operands[0].reg) {
								#define CREG(lowercase, uppercase) \
									case PPC_REG_##uppercase: { \
										_[ jump(lowercase##gt, directSuccessor(), then.basicBlock()) ]; \
									} break;
									CREG(cr2, CR2);
									CREG(cr3, CR3);
									CREG(cr4, CR4);
									CREG(cr5, CR5);
									CREG(cr6, CR6);
									CREG(cr7, CR7);
								}
							} else {
								then[jump(operand(0))];
								_[ jump(cr0gt, directSuccessor(), then.basicBlock()) ];
							}
						}
						break;
					case PPC_BC_GE: {
							PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
							if (detail_->op_count > 1) {
								then[jump(operand(1))];
								switch (detail_->operands[0].reg) {
								#define CREG(lowercase, uppercase) \
									case PPC_REG_##uppercase: { \
										_[ jump(lowercase##lt, directSuccessor(), then.basicBlock()) ]; \
									} break;
									CREG(cr2, CR2);
									CREG(cr3, CR3);
									CREG(cr4, CR4);
									CREG(cr5, CR5);
									CREG(cr6, CR6);
									CREG(cr7, CR7);
								}
							} else {
								then[jump(operand(0))];
								_[ jump(cr0lt, directSuccessor(), then.basicBlock()) ];
							}
						}
						break;
					case PPC_BC_GT:
							if (detail_->op_count > 1) {
								switch (detail_->operands[0].reg) {
								#define CREG(lowercase, uppercase) \
									case PPC_REG_##uppercase: { \
										_[ jump(lowercase##gt, operand(1), directSuccessor()) ]; \
									} break;
									CREG(cr2, CR2);
									CREG(cr3, CR3);
									CREG(cr4, CR4);
									CREG(cr5, CR5);
									CREG(cr6, CR6);
									CREG(cr7, CR7);
								}
							} else {
								_[ jump(cr0gt, operand(0), directSuccessor()) ];
							}
						break;
					case PPC_BC_EQ:
							if (detail_->op_count > 1) {
								switch (detail_->operands[0].reg) {
								#define CREG(lowercase, uppercase) \
									case PPC_REG_##uppercase: { \
										_[ jump(lowercase##eq, operand(1), directSuccessor()) ]; \
									} break;
									CREG(cr2, CR2);
									CREG(cr3, CR3);
									CREG(cr4, CR4);
									CREG(cr5, CR5);
									CREG(cr6, CR6);
									CREG(cr7, CR7);
								}
							} else {
								_[ jump(cr0eq, operand(0), directSuccessor()) ];
							}
						break;
					case PPC_BC_NE:
							if (detail_->op_count > 1) {
								switch (detail_->operands[0].reg) {
								#define CREG(lowercase, uppercase) \
									case PPC_REG_##uppercase: { \
										_[ jump(~lowercase##eq, operand(1), directSuccessor()) ]; \
									} break;
									CREG(cr2, CR2);
									CREG(cr3, CR3);
									CREG(cr4, CR4);
									CREG(cr5, CR5);
									CREG(cr6, CR6);
									CREG(cr7, CR7);
								}
							} else {
								_[ jump(~cr0eq, operand(0), directSuccessor()) ];
							}
						break;
					default:
						_(std::make_unique<core::ir::InlineAssembly>());
						break;
				}
			}
			break;
		case PPC_INS_BL: {
				_[call(operand(0))];
			}
			break;
		case PPC_INS_BLR: {
				_[jump(regizter(PPCRegisters::lr()))];
			}
			break;
		case PPC_INS_BCTR: {
				_[call(regizter(PPCRegisters::ctr()))];
			}
			break;
		case PPC_INS_MTLR: {
				_[regizter(PPCRegisters::lr()) ^= operand(0,64)];
			}
			break;
		case PPC_INS_MFLR: {
				_[operand(0,64) ^= regizter(PPCRegisters::lr())];
			}
			break;
		case PPC_INS_MTCTR: {
				_[regizter(PPCRegisters::ctr()) ^= operand(0)];
			}
			break;
		case PPC_INS_CMPWI:
		case PPC_INS_CMPDI:
		case PPC_INS_CMPW:
		case PPC_INS_CMPD:
		case PPC_INS_CMPLWI:
		case PPC_INS_CMPLDI:
		case PPC_INS_CMPLW:
		case PPC_INS_CMPLD: {
			if (detail_->op_count > 2 && detail_->operands[0].type == PPC_OP_REG) {
				unsigned int reg = detail_->operands[0].reg;
#if 1
				switch (detail_->operands[0].reg) {
					#define CREG(lowercase, uppercase) \
						case PPC_REG_##uppercase: { \
							_[ \
								lowercase##lt ^= signed_(operand(1)) < operand(2), \
								lowercase##gt ^= signed_(operand(1)) > operand(2), \
								lowercase##eq ^= unsigned_(operand(1)) == operand(2) \
							]; \
						} break;
					CREG(cr2, CR2);
					CREG(cr3, CR3)
					CREG(cr4, CR4)
					CREG(cr5, CR5)
					CREG(cr6, CR6)
					CREG(cr7, CR7)
						default:
							break;
				} /* end switch */
#endif
			}
			else {
				_[
					cr0lt ^= signed_(operand(0)) < operand(1),
					cr0gt ^= signed_(operand(0)) > operand(1),
					cr0eq ^= unsigned_(operand(0)) == operand(1)
				];
			}
			}
			break;
		case PPC_INS_DCBT: // TODO
		case PPC_INS_NOP: {
			}
			break;
		case PPC_INS_AND:
		case PPC_INS_ANDI: {
				_[operand(0) ^= (operand(1) & operand(2))];
			}
			break;
		case PPC_INS_ANDIS: {
				_[operand(0) ^= (operand(1) & (operand(2) << constant(16)))];
			}
			break;
		case PPC_INS_OR:
		case PPC_INS_ORI: {
				_[operand(0) ^= (operand(1) | (operand(2)))];
			}
			break;
		case PPC_INS_ORIS: {
				_[operand(0) ^= (operand(1) | unsigned_(operand(2) << constant(16)))];
			}
			break;
		case PPC_INS_ADDC: // TODO
		case PPC_INS_ADD:
		case PPC_INS_ADDI: {
				_[operand(0) ^= (operand(1) + operand(2))];
			}
			break;
		case PPC_INS_ADDIS: {
				_[operand(0) ^= (operand(1) + (operand(2) << constant(16)))];
			}
			break;
		case PPC_INS_XOR:
		case PPC_INS_XORI: {
				_[operand(0) ^= (operand(1) ^ operand(2))];
			}
			break;
		case PPC_INS_XORIS: {
				_[operand(0) ^= (operand(1) ^ (operand(2) << constant(16)))];
			}
			break;
		case PPC_INS_NAND: {
				_[operand(0) ^= ~(operand(1) & operand(2))];
			}
			break;
		case PPC_INS_NOR: {
				_[operand(0) ^= ~(operand(1) | operand(2))];
			}
			break;
		case PPC_INS_LIS: {
				_[operand(0) ^= (operand(1) << constant(16))];
			}
			break;
		case PPC_INS_NEG: {
				_[operand(0) ^= -operand(1)];
			}
			break;
		case PPC_INS_CLRLDI: // TODO
		case PPC_INS_EXTSW: // TODO
		case PPC_INS_LI:
		case PPC_INS_MR: {
				_[operand(0) ^= operand(1)];
			}
			break;
		case PPC_INS_STDU: { // stdu RS, D(RA) : EA = RA + D, [EA] = RS, RA = EA
				_(std::make_unique<core::ir::InlineAssembly>());
			}
			break;
		case PPC_INS_STD:
		case PPC_INS_STW: { // operand 1 = first register, operand 2 = { offset, base register }
				_(std::make_unique<core::ir::InlineAssembly>());
			}
			break;
		case PPC_INS_LD:
		case PPC_INS_LWZ: {
				_(std::make_unique<core::ir::InlineAssembly>());
			}
			break;
		default: {
                _(std::make_unique<core::ir::InlineAssembly>());
                break;
			}
		}
	}

    core::arch::CapstoneInstructionPtr disassemble(const PPCInstruction *instruction) {
        capstone_.setMode(instruction->csMode());
        return capstone_.disassemble(instruction->addr(), instruction->bytes(), instruction->size());
    }

	std::unique_ptr<core::ir::Dereference> createDereference(const cs_ppc_op &operand) const {
        return std::make_unique<core::ir::Dereference>(
            createDereferenceAddress(operand), core::ir::MemoryDomain::MEMORY, 32);
    }

    std::unique_ptr<core::ir::Term> createDereferenceAddress(const cs_ppc_op &operand) const {
        if (operand.type != PPC_OP_MEM) {
            throw core::irgen::InvalidInstructionException(tr("Expected the operand to be a memory operand"));
        }

        const auto &mem = operand.mem;

        auto result = createRegisterAccess(mem.base);
        auto offsetValue = SizedValue(result->size(), mem.disp);

        if (offsetValue.value() || !result) {
            auto offset = std::make_unique<core::ir::Constant>(offsetValue);

            if (result) {
                result = std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, std::move(result), std::move(offset), result->size());
            } else {
                result = std::move(offset);
            }
        }
        
        return result;
    }

	core::irgen::expressions::TermExpression operand(std::size_t index, SmallBitSize sizeHint = 32) const {
        return core::irgen::expressions::TermExpression(createTermForOperand(index, sizeHint));
    }

	std::unique_ptr<core::ir::Term> createTermForOperand(std::size_t index, SmallBitSize sizeHint) const {
        assert(index < boost::size(detail_->operands));

        const auto &operand = detail_->operands[index];
        
        switch (operand.type) {
            case PPC_OP_INVALID:
                throw core::irgen::InvalidInstructionException(tr("The instruction does not have an argument with index %1").arg(index));
            case PPC_OP_REG: {
                return std::make_unique<core::ir::MemoryLocationAccess>(getRegister(operand.reg)->memoryLocation().resized(sizeHint));
            }
            case PPC_OP_IMM: {
                /* Immediate value. */
                return std::make_unique<core::ir::Constant>(SizedValue(sizeHint, operand.imm));
            }
            case PPC_OP_MEM: {
                return std::make_unique<core::ir::Dereference>(createDereferenceAddress(operand), core::ir::MemoryDomain::MEMORY, sizeHint);
            }
            default:
                unreachable();
        }
    }

	static std::unique_ptr<core::ir::Term> createRegisterAccess(int reg) {
        return PPCInstructionAnalyzer::createTerm(getRegister(reg));
    }

	static const core::arch::Register *getRegister(int reg) {
        switch (reg) {
        #define REG(uppercase, lowercase) \
            case PPC_REG_##uppercase: return PPCRegisters::lowercase();
			REG(CR0, cr0)
			REG(CR1, cr1)
			REG(CR2, cr2)
			REG(CR3, cr3)
			REG(CR4, cr4)
			REG(CR5, cr5)
			REG(CR6, cr6)
			REG(CR7, cr7)

			REG(R0, r0)
			REG(R1, r1)
			REG(R2, r2)
			REG(R3, r3)
			REG(R4, r4)
			REG(R5, r5)
			REG(R6, r6)
			REG(R7, r7)
			REG(R8, r8)
			REG(R9, r9)
			REG(R10, r10)
			REG(R11, r11)
			REG(R12, r12)
			REG(R13, r13)
			REG(R14, r14)
			REG(R15, r15)
			REG(R16, r16)
			REG(R17, r17)
			REG(R18, r18)
			REG(R19, r19)
			REG(R20, r20)
			REG(R21, r21)
			REG(R22, r22)
			REG(R23, r23)
			REG(R24, r24)
			REG(R25, r25)
			REG(R26, r26)
			REG(R27, r27)
			REG(R28, r28)
			REG(R29, r29)
			REG(R30, r30)
			REG(R31, r31)
            default:
            throw core::irgen::InvalidInstructionException(tr("Invalid register number: %1").arg(reg));
        }
    }
};

PPCInstructionAnalyzer::PPCInstructionAnalyzer(const PPCArchitecture *architecture):
    impl_(std::make_unique<PPCInstructionAnalyzerImpl>(architecture))
{}

PPCInstructionAnalyzer::~PPCInstructionAnalyzer() {}

void PPCInstructionAnalyzer::doCreateStatements(const core::arch::Instruction *instruction, core::ir::Program *program) {
    impl_->createStatements(checked_cast<const PPCInstruction *>(instruction), program);
}

}}} // namespace nc::arch::ppc

/* vim:set et sts=4 sw=4: */
