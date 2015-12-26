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
    #define CREG_(lowercase, uppercase, condition, thenTarget, elseTarget) \
        case PPC_REG_##uppercase: { \
            _[ jump(lowercase ## condition, thenTarget, elseTarget) ]; \
        } break;

    #define NCREG_(lowercase, uppercase, condition, thenTarget, elseTarget) \
        case PPC_REG_##uppercase: { \
            _[ jump(~lowercase ## condition, thenTarget, elseTarget) ]; \
       } break;

    #define CCASE(condition, thenTarget, elseTarget) \
        switch (detail_->operands[0].reg) { \
            CREG_(cr2, CR2, condition, thenTarget, elseTarget) \
            CREG_(cr3, CR3, condition, thenTarget, elseTarget) \
            CREG_(cr4, CR4, condition, thenTarget, elseTarget) \
            CREG_(cr5, CR5, condition, thenTarget, elseTarget) \
            CREG_(cr6, CR6, condition, thenTarget, elseTarget) \
            CREG_(cr7, CR7, condition, thenTarget, elseTarget) \
        }
    
    #define NCCASE(condition, thenTarget, elseTarget) \
        switch (detail_->operands[0].reg) { \
            NCREG_(cr2, CR2, condition, thenTarget, elseTarget) \
            NCREG_(cr3, CR3, condition, thenTarget, elseTarget) \
            NCREG_(cr4, CR4, condition, thenTarget, elseTarget) \
            NCREG_(cr5, CR5, condition, thenTarget, elseTarget) \
            NCREG_(cr6, CR6, condition, thenTarget, elseTarget) \
            NCREG_(cr7, CR7, condition, thenTarget, elseTarget) \
        }

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

        auto mask32 = [&](int mb, int me) -> unsigned int {
            auto mask = 0;
            for (int i = mb; i <= me; i++) {
                mask |= (1<<(31-i));
            }
            return mask;
        };

        auto mask64 = [&](int mb, int me) -> unsigned int {
            auto mask = 0;
            for (int i = mb; i <= me; i++) {
                mask |= (1<<(63-i));
            }
            return mask;
        };

        using namespace core::irgen::expressions;

        //_[regizter(PPCRegisters::r2()) ^= constant(0x28438)]; // set RTOC here

        switch (instr->id) {
            /* branch with link register NOT set */
        case PPC_INS_B:
        case PPC_INS_BA:
        case PPC_INS_BC: {
                switch (detail_->bc) {
                    case PPC_BC_INVALID:
                        _[jump(operand(0))];
                        break;
                    case PPC_BC_LT:
                            if (detail_->op_count > 1) {
                                CCASE(lt, operand(1), directSuccessor())
                            } else {
                                _[ jump(cr0lt, operand(0), directSuccessor()) ];
                            }
                        break;
                    case PPC_BC_LE: {
                            PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                            if (detail_->op_count > 1) {
                                then[jump(operand(1))];
                                CCASE(gt, directSuccessor(), then.basicBlock())
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
                                CCASE(lt, directSuccessor(), then.basicBlock())
                            } else {
                                then[jump(operand(0))];
                                _[ jump(cr0lt, directSuccessor(), then.basicBlock()) ];
                            }
                        }
                        break;
                    case PPC_BC_GT:
                            if (detail_->op_count > 1) {
                                CCASE(gt, operand(1), directSuccessor())
                            } else {
                                _[ jump(cr0gt, operand(0), directSuccessor()) ];
                            }
                        break;
                    case PPC_BC_EQ:
                            if (detail_->op_count > 1) {
                                CCASE(eq, operand(1), directSuccessor())
                            } else {
                                _[ jump(cr0eq, operand(0), directSuccessor()) ];
                            }
                        break;
                    case PPC_BC_NE:
                            if (detail_->op_count > 1) {
                                NCCASE(eq, operand(1), directSuccessor())
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
            /* branch to LR with link register NOT set */
        case PPC_INS_BLR:
        case PPC_INS_BCLR: {
            switch (detail_->bc) {
                case PPC_BC_INVALID:
                        _[jump(regizter(PPCRegisters::lr()))];
                        break;
                case PPC_BC_LT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[jump(regizter(PPCRegisters::lr()))];
                        if (detail_->op_count == 1) {
                            CCASE(lt, directSuccessor(), then.basicBlock())
                        } else {
                            _[jump(cr0lt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_LE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[jump(regizter(PPCRegisters::lr()))];
                        if (detail_->op_count == 1) {
                            CCASE(gt, directSuccessor(), then.basicBlock())
                        } else {
                            _[jump(cr0gt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_GE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[jump(regizter(PPCRegisters::lr()))];
                        if (detail_->op_count == 1) {
                            CCASE(lt, directSuccessor(), then.basicBlock())
                        } else {
                            _[jump(cr0lt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_GT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[jump(regizter(PPCRegisters::lr()))];
                        if (detail_->op_count == 1) {
                            CCASE(gt, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0gt, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_EQ: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ jump(regizter(PPCRegisters::lr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_NE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ jump(regizter(PPCRegisters::lr())) ];
                        if (detail_->op_count == 1) {
                            NCCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(~cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                default:
                    _(std::make_unique<core::ir::InlineAssembly>());
                    break;
                } /* switch */
            }
            break;
            /* branch to CTR register with link register NOT set */
        case PPC_INS_BCTR:
        case PPC_INS_BCCTR: {
            switch (detail_->bc) {
                case PPC_BC_INVALID: {
                        _[jump(regizter(PPCRegisters::ctr()))];
                    }
                    break;
                case PPC_BC_LT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ jump(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(lt, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0lt, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_LE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ jump(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(gt, directSuccessor(), then.basicBlock())
                        } else {
                            _[jump(cr0gt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_GT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ jump(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(gt, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0gt, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_GE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ jump(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(lt, directSuccessor(), then.basicBlock())
                        } else {
                            _[jump(cr0lt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_EQ: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ jump(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_NE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ jump(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            NCCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(~cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                default:
                    _(std::make_unique<core::ir::InlineAssembly>());
                    break;
                } /* switch */
            }
            break;
            /* branch with link register set */
        case PPC_INS_BL:
        case PPC_INS_BLA:
        case PPC_INS_BCL: {
            switch (detail_->bc) {
                case PPC_BC_INVALID:
                    _[call(operand(0))];
                    break;
                case PPC_BC_LT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        if (detail_->op_count > 1) {
                            then[call(operand(1))];
                            CCASE(lt, then.basicBlock(), directSuccessor())
                        } else {
                            then[call(operand(0))];
                            _[jump(cr0lt, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_LE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        if (detail_->op_count > 1) {
                            then[call(operand(1))];
                            CCASE(gt, directSuccessor(), then.basicBlock())
                        } else {
                            then[call(operand(0))];
                            _[jump(cr0gt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_GT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        if (detail_->op_count > 1) {
                            then[call(operand(1))];
                            CCASE(gt, then.basicBlock(), directSuccessor())
                        } else {
                            then[call(operand(0))];
                            _[jump(cr0gt, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_GE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        if (detail_->op_count > 1) {
                            then[call(operand(1))];
                            CCASE(lt, directSuccessor(), then.basicBlock())
                        } else {
                            then[call(operand(0))];
                            _[jump(cr0lt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_EQ: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        if (detail_->op_count > 1) {
                            then[call(operand(1))];
                            CCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            then[call(operand(0))];
                            _[jump(cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_NE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        if (detail_->op_count > 1) {
                            then[call(operand(1))];
                            NCCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            then[call(operand(0))];
                            _[jump(~cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                default:
                    _(std::make_unique<core::ir::InlineAssembly>());
                    break;
                } /* switch */
            }
            break;
            /* branch to LR with link register set */
        case PPC_INS_BLRL:
        case PPC_INS_BCLRL: {
            switch (detail_->bc) {
                case PPC_BC_INVALID: {
                        _[ call(regizter(PPCRegisters::lr())) ];
                    }
                    break;
                case PPC_BC_LT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::lr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(lt, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0lt, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_LE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::lr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(gt, directSuccessor(), then.basicBlock())
                        } else {
                            _[jump(cr0gt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_GT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::lr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(gt, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0gt, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_GE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::lr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(lt, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0lt, then.basicBlock(), directSuccessor())];
                        }
                    }
                case PPC_BC_EQ: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::lr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_NE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::lr())) ];
                        if (detail_->op_count == 1) {
                            NCCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                default:
                    _(std::make_unique<core::ir::InlineAssembly>());
                    break;
                } /* switch */
            }
            break;
            /* branch to CTR with link register set */
        case PPC_INS_BCTRL:
        case PPC_INS_BCCTRL: {
            switch (detail_->bc) {
                case PPC_BC_INVALID: {
                        _[ call(regizter(PPCRegisters::ctr())) ];
                    }
                    break;
                case PPC_BC_LT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(lt, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0lt, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_LE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(gt, directSuccessor(), then.basicBlock())
                        } else {
                            _[jump(cr0gt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_GT: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(gt, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0gt, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_GE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(lt, directSuccessor(), then.basicBlock())
                        } else {
                            _[jump(cr0lt, directSuccessor(), then.basicBlock())];
                        }
                    }
                    break;
                case PPC_BC_EQ: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            CCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                case PPC_BC_NE: {
                        PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                        then[ call(regizter(PPCRegisters::ctr())) ];
                        if (detail_->op_count == 1) {
                            NCCASE(eq, then.basicBlock(), directSuccessor())
                        } else {
                            _[jump(~cr0eq, then.basicBlock(), directSuccessor())];
                        }
                    }
                    break;
                default:
                    _(std::make_unique<core::ir::InlineAssembly>());
                    break;
                } /* switch */
            }
            break;
        case PPC_INS_BDNZ:
        case PPC_INS_BDNZA: {
                _[
                    regizter(PPCRegisters::ctr()) ^= regizter(PPCRegisters::ctr()) - constant(1),
                    jump(~(regizter(PPCRegisters::ctr()) == constant(0)), operand(0), directSuccessor())
                ];
            }
            break;
        case PPC_INS_BDNZLR: {
                PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                then[ jump(regizter(PPCRegisters::lr())) ];
                _[
                    regizter(PPCRegisters::ctr()) ^= regizter(PPCRegisters::ctr()) - constant(1),
                    jump(~(regizter(PPCRegisters::ctr()) == constant(0)), then.basicBlock(), directSuccessor())
                ];
            }
            break;
        case PPC_INS_BDNZL:
        case PPC_INS_BDNZLA:{
                PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                then[ call(operand(0)) ];
                _[
                    regizter(PPCRegisters::ctr()) ^= regizter(PPCRegisters::ctr()) - constant(1),
                    jump(~(regizter(PPCRegisters::ctr()) == constant(0)), then.basicBlock(), directSuccessor())
                ];
            }
            break;
        case PPC_INS_BDNZLRL: {
                PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                then[ call(regizter(PPCRegisters::lr())) ];
                _[
                    regizter(PPCRegisters::ctr()) ^= regizter(PPCRegisters::ctr()) - constant(1),
                    jump(~(regizter(PPCRegisters::ctr()) == constant(0)), then.basicBlock(), directSuccessor())
                ];
            }
            break;
        case PPC_INS_BDZ:
        case PPC_INS_BDZA: {
                _[
                    regizter(PPCRegisters::ctr()) ^= regizter(PPCRegisters::ctr()) - constant(1),
                    jump(regizter(PPCRegisters::ctr()) == constant(0), operand(0), directSuccessor())
                ];
            }
            break;
        case PPC_INS_BDZLR: {
                PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                then[ jump(regizter(PPCRegisters::lr())) ];
                _[
                    regizter(PPCRegisters::ctr()) ^= regizter(PPCRegisters::ctr()) - constant(1),
                    jump(regizter(PPCRegisters::ctr()) == constant(0), then.basicBlock(), directSuccessor())
                ];
            }
            break;
        case PPC_INS_BDZL:
        case PPC_INS_BDZLA: {
                PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                then[ call(operand(0)) ];
                _[
                    regizter(PPCRegisters::ctr()) ^= regizter(PPCRegisters::ctr()) - constant(1),
                    jump(regizter(PPCRegisters::ctr()) == constant(0), then.basicBlock(), directSuccessor())
                ];
            }
            break;
        case PPC_INS_BDZLRL: {
                PPCExpressionFactoryCallback then(factory_, program->createBasicBlock(), instruction);
                then[ call(regizter(PPCRegisters::lr())) ];
                _[
                    regizter(PPCRegisters::ctr()) ^= regizter(PPCRegisters::ctr()) - constant(1),
                    jump(regizter(PPCRegisters::ctr()) == constant(0), then.basicBlock(), directSuccessor())
                ];
            }
            break;
        case PPC_INS_MTLR: {
                _[regizter(PPCRegisters::lr()) ^= operand(0)];
            }
            break;
        case PPC_INS_MFLR: {
                _[operand(0) ^= regizter(PPCRegisters::lr())];
            }
            break;
        case PPC_INS_MTCTR: {
                _[regizter(PPCRegisters::ctr()) ^= operand(0)];
            }
            break;
        case PPC_INS_MFCR: {
                _[operand(0) ^= regizter(PPCRegisters::cr())];
            }
            break;
        case PPC_INS_MTOCRF: { // TODO: missing FXM operand from Capstone
                _(std::make_unique<core::ir::InlineAssembly>());
            }
            break;
        case PPC_INS_MFOCRF: { // TODO: missing FXM operand from Capstone
                _(std::make_unique<core::ir::InlineAssembly>());
            }
            break;
        case PPC_INS_SC: {
                // hopefully temporary
                _[call(regizter(PPCRegisters::r11()))];
                //_[call(regizter(PPCRegisters::r0()))];
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
            } else {
                _[
                    cr0lt ^= signed_(operand(0)) < operand(1),
                    cr0gt ^= signed_(operand(0)) > operand(1),
                    cr0eq ^= unsigned_(operand(0)) == operand(1)
                ];
                }
            }
            break;
        case PPC_INS_NOP: {
            }
            break;
        case PPC_INS_ANDC: { // TODO
                _[operand(0) ^= (operand(1) & ~operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_AND: {
                _[operand(0) ^= (operand(1) & operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_ANDI: {
                _[operand(0) ^= (operand(1) & operand(2))];
            }
            break;
        case PPC_INS_ANDIS: {
                _[operand(0) ^= (operand(1) & (operand(2) << constant(16)))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_ORI: {
                _[operand(0) ^= (operand(1) | operand(2))];
            }
            break;
        case PPC_INS_ORC: {
                _[operand(0) ^= (operand(1) | ~operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_OR: {
                if (detail_->operands[1].reg == detail_->operands[2].reg) { // MR
                    _[operand(0) ^= operand(1)];
                } else {
                    _[operand(0) ^= (operand(1) | operand(2))];
                }
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_ORIS: {
                _[operand(0) ^= (operand(1) | unsigned_(operand(2) << constant(16)))];
            }
            break;
        case PPC_INS_ADDIC: // TODO
        case PPC_INS_ADDI: {
                _[operand(0) ^= (operand(1) + signed_(operand(2)))];
            }
            break;
        case PPC_INS_ADDE: // TODO
        case PPC_INS_ADDC: // TODO
        case PPC_INS_ADD: {
                _[operand(0) ^= (operand(1) + operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_ADDZE: { // TODO
                _[operand(0) ^= operand(1)]; // + CARRY FLAG
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_ADDIS: {
                _[operand(0) ^= (operand(1) + (operand(2) << constant(16)))];
            }
            break;
        case PPC_INS_SUBFIC: {
                _[operand(0) ^= operand(1) - operand(2)];
            }
            break;
        case PPC_INS_SUB:
        case PPC_INS_SUBC: // TODO
        case PPC_INS_SUBF:
        case PPC_INS_SUBFC: { // TODO
                _[operand(0) ^= operand(1) - operand(2)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_XOR: {
                _[operand(0) ^= (operand(1) ^ operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
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
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_NOR: {
                _[operand(0) ^= ~(operand(1) | operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_EQV: {
                //_[operand(0) ^= ~(operand(1) ^ operand(2))];
                _[operand(0) ^= (operand(1) == operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_NEG: {
                _[operand(0) ^= -operand(1)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_SLD:
        case PPC_INS_SLW: {
                _[operand(0) ^= operand(1) << operand(2)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_SRD:
        case PPC_INS_SRW: {
                _[operand(0) ^= (signed_(operand(1)) >> operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_MULLI: {
                _[operand(0) ^= operand(1) * operand(2)];
            }
            break;
        case PPC_INS_MULLW:
        case PPC_INS_MULLD: {
                _[operand(0) ^= operand(1) * operand(2)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_MULHD: {
                _[operand(0) ^= signed_(operand(1) * operand(2)) >> constant(64)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_MULHW: {
                _[operand(0) ^= signed_(operand(1) * operand(2)) >> constant(32)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_MULHDU: {
                _[operand(0) ^= unsigned_(operand(1) * operand(2)) >> constant(64)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_MULHWU: {
                _[operand(0) ^= unsigned_(operand(1) * operand(2)) >> constant(32)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_DIVW:
        case PPC_INS_DIVD: {
                _[operand(0) ^= signed_(operand(1)) / operand(2)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_DIVWU:
        case PPC_INS_DIVDU: {
                _[operand(0) ^= unsigned_(operand(1)) / operand(2)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_RLDICL: {
                // mnemonics (the ones not handled by Capstone)
                if ((64 - detail_->operands[2].imm) == detail_->operands[3].imm) { // SRDI
                    _[operand(0) ^= (signed_(operand(1)) >> operand(3))];
                //} else if () {// EXTRDI
                //} else if () {// ROTRDI
                } else {
                    auto mask = mask64(detail_->operands[3].imm,63);
                    _[operand(0) ^= (unsigned_(operand(1)) >> operand(2) | operand(1) << (constant(64) - operand(2))) & constant(mask)];
                    //_(std::make_unique<core::ir::InlineAssembly>());
                }
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_RLDICR: {
                auto mask = mask64(0, detail_->operands[3].imm);
                _[operand(0) ^= (unsigned_(operand(1)) >> operand(2) | operand(1) << (constant(64) - operand(2))) & constant(mask)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_RLDIC: {
                auto mask = mask64(detail_->operands[3].imm, 63 - detail_->operands[2].imm);
                _[operand(0) ^= (unsigned_(operand(1)) >> operand(2) | operand(1) << (constant(64) - operand(2))) & constant(mask)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_ROTLD:
        case PPC_INS_ROTLDI: {
                _[operand(0) ^= (operand(1) << operand(2) | unsigned_(operand(1)) >> (constant(64) - operand(2)))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_RLWIMI: { //(ra & ~mask) | (rotl32(rs, sh) & mask);
                auto mask = mask32(detail_->operands[3].imm, detail_->operands[4].imm);
                _[operand(0) ^= ((unsigned_(operand(1)) << operand(2) | unsigned_(operand(1)) >> (constant(32) - operand(2))) & constant(mask))
                    | (operand(1) & ~constant(mask))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_RLWNM: // 32 bit
        case PPC_INS_RLWINM: {
                auto mask = mask32(detail_->operands[3].imm, detail_->operands[4].imm);
                if (detail_->operands[2].imm > 0) {
                    _[operand(0) ^= (unsigned_(operand(1)) << operand(2) | unsigned_(operand(1)) >> (constant(32) - operand(2))) & constant(mask)];
                } else {
                    _[operand(0) ^= operand(1) & constant(mask)];
                }
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_RLDCL: {
                auto mask = mask64(detail_->operands[3].imm, 63);
                _[operand(0) ^= (unsigned_(operand(1)) << operand(2) | unsigned_(operand(1)) >> (constant(64) - operand(2))) & constant(mask)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_RLDCR: {
                auto mask = mask64(0, detail_->operands[3].imm);
                _[operand(0) ^= (unsigned_(operand(1)) << operand(2) | unsigned_(operand(1)) >> (constant(64) - operand(2))) & constant(mask)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_ROTLW:
        case PPC_INS_ROTLWI: {
                _[operand(0) ^= (operand(1) << operand(2) | unsigned_(operand(1)) >> (constant(32) - operand(2)))];
            }
            break;
        case PPC_INS_SRWI: {
                _[operand(0) ^= (operand(1) << operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_SRAW: /* TODO */
        case PPC_INS_SRAD:
        case PPC_INS_SRAWI:
        case PPC_INS_SRADI: {
                _[operand(0) ^= (operand(1) << operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_SLWI: {
                _[operand(0) ^= (operand(1) << operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_SLDI: { // TODO: missing 3rd operand from Capstone
                //_(std::make_unique<core::ir::InlineAssembly>());
                /*_[operand(0) ^= (operand(1) << operand(2))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }*/
            }
            break;
        case PPC_INS_EXTSB: {
                _[operand(0) ^= sign_extend(operand(1,8))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_EXTSH: {
                _[operand(0) ^= sign_extend(operand(1,16))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_EXTSW: {
                _[operand(0) ^= sign_extend(operand(1,32))];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_CLRLWI: // TODO
        case PPC_INS_CLRLDI: // TODO
        case PPC_INS_LI: {
                _[operand(0) ^= operand(1)];
            }
            break;
        case PPC_INS_LIS: {
                _[operand(0) ^= (operand(1) << constant(16))];
            }
            break;
        case PPC_INS_MR: {
                _[operand(0) ^= operand(1)];
                if (detail_->update_cr0) {
                    _[cr0lt ^= signed_(operand(0)) < constant(0),
                      cr0gt ^= signed_(operand(0)) > constant(0),
                      cr0eq ^= unsigned_(operand(0)) == constant(0)];
                }
            }
            break;
        case PPC_INS_STDX: {
                _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 64)) ^= operand(0)];
            }
            break;
        case PPC_INS_STDUX: {
                _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 64)) ^=  operand(0),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64))];
            }
            break;
        case PPC_INS_STDU: {
                _[core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 64)) ^=  operand(0),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, detail_->operands[1].mem.disp)), 64))];
            }
            break;
        case PPC_INS_STD: {
                _[core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 64)) ^=  operand(0)];
            }
            break;
        case PPC_INS_STWX: {
                _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 32)) ^= truncate(operand(0), 32)];
            }
            break;
        case PPC_INS_STWUX: {
                _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 32)) ^=  truncate(operand(0), 32),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64))];
            }
            break;
        case PPC_INS_STWU: {
                _[core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 32)) ^=  truncate(operand(0), 32),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, detail_->operands[1].mem.disp)), 64))];
            }
            break;
        case PPC_INS_STW: { 
                _[core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 32)) ^= truncate(operand(0), 32)];
            }
            break;
        case PPC_INS_STHX: {
                _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 16)) ^= truncate(operand(0), 16)];
            }
            break;
        case PPC_INS_STHUX: {
                _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 16)) ^= truncate(operand(0), 16),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64))];
            }
            break;
        case PPC_INS_STHU: {
                _[core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 16)) ^= truncate(operand(0), 16),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, detail_->operands[1].mem.disp)), 64))];
            }
            break;
        case PPC_INS_STH: {
                _[core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 16)) ^= truncate(operand(0), 16)];
            }
            break;
        case PPC_INS_STBX: {
                _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 8)) ^= truncate(operand(0), 8)];
            }
            break;
        case PPC_INS_STBUX: {
                _[core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 8)) ^=  truncate(operand(0), 8),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64))];
            }
            break;
        case PPC_INS_STBU: {
                _[core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 8)) ^=  truncate(operand(0), 8),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, detail_->operands[1].mem.disp)), 64))];
            }
            break;
        case PPC_INS_STB: {
                _[core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 8)) ^= truncate(operand(0), 8)];
            }
            break;
        case PPC_INS_LDX: {
                _[operand(0) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 64))];
            }
            break;
        case PPC_INS_LDUX: {
                _[operand(0) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 64)),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), 64))];
            }
            break;
        case PPC_INS_LDU: {
                _[operand(0) ^= core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 64)),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, detail_->operands[1].mem.disp)), 64))];
            }
            break;
        case PPC_INS_LD: {
                _[operand(0) ^= core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 64))];
            }
            break;
        case PPC_INS_LWZX: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 32)))];
            }
            break;
        case PPC_INS_LWZUX: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 32))),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64))];
            }
            break;
        case PPC_INS_LWZU: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 32))),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, detail_->operands[1].mem.disp)), 64))];
            }
            break;
        case PPC_INS_LWZ: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 32)))];
            }
            break;
        case PPC_INS_LWAX: {
                _[operand(0) ^= sign_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 32)))];
            }
            break;
        case PPC_INS_LWAUX: {
                _[operand(0) ^= sign_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 32))),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64))];
            }
            break;
        case PPC_INS_LWA: {
                _[operand(0) ^= sign_extend(core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 32)))];
            }
            break;
        case PPC_INS_LHZX: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 16)))];
            }
            break;
        case PPC_INS_LHZUX: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 16))),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64))];
            }
            break;
        case PPC_INS_LHZU: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 16))),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, detail_->operands[1].mem.disp)), 64))];
            }
            break;
        case PPC_INS_LHZ: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 16)))];
            }
            break;
        case PPC_INS_LHAX: {
                _[operand(0) ^= sign_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 16)))];
            }
            break;
        case PPC_INS_LHAUX: {
                _[operand(0) ^= sign_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 16))),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64))];
            }
            break;
        case PPC_INS_LHAU: {
                _[operand(0) ^= sign_extend(core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 16))),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, detail_->operands[1].mem.disp)), 64))];
            }
            break;
        case PPC_INS_LHA: {
                _[operand(0) ^= sign_extend(core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 16)))];
            }
            break;
        case PPC_INS_LBZX: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 8)))];
            }
            break;
        case PPC_INS_LBZUX: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64), core::ir::MemoryDomain::MEMORY, 8))),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].reg)), PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[2].reg)), 64))];
            }
            break;
        case PPC_INS_LBZU: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 8))),
                 core::irgen::expressions::TermExpression(std::make_unique<core::ir::MemoryLocationAccess>(getRegister(detail_->operands[1].reg)->memoryLocation().resized(64))) ^= core::irgen::expressions::TermExpression(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(detail_->operands[1].mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, detail_->operands[1].mem.disp)), 64))];
            }
            break;
        case PPC_INS_LBZ: {
                _[operand(0) ^= zero_extend(core::irgen::expressions::TermExpression(createDereferenceAddress(detail_->operands[1], 8)))];
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

    std::unique_ptr<core::ir::Dereference> createDereference(const cs_ppc_op &operand, SmallBitSize sizeHint) const {
        return std::make_unique<core::ir::Dereference>(
                   createDereferenceAddress(operand, sizeHint), core::ir::MemoryDomain::MEMORY, sizeHint);
    }

    std::unique_ptr<core::ir::Dereference> createDereferenceAddress(const cs_ppc_op &operand, SmallBitSize sizeHint) const {
        if (operand.type != PPC_OP_MEM) {
            throw core::irgen::InvalidInstructionException(tr("Expected the operand to be a memory operand"));
        }

            if (operand.mem.disp) {
                return std::make_unique<core::ir::Dereference>(std::make_unique<core::ir::BinaryOperator>(core::ir::BinaryOperator::ADD, PPCInstructionAnalyzer::createTerm(getRegister(operand.mem.base)), std::make_unique<core::ir::Constant>(SizedValue(64, operand.mem.disp)), 64), core::ir::MemoryDomain::MEMORY, sizeHint);
            } else {
                return std::make_unique<core::ir::Dereference>(PPCInstructionAnalyzer::createTerm(getRegister(operand.mem.base)), core::ir::MemoryDomain::MEMORY, sizeHint);
            }
    }

    core::irgen::expressions::TermExpression operand(std::size_t index, SmallBitSize sizeHint = 64) const {
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
            return std::make_unique<core::ir::Dereference>(createDereferenceAddress(operand, sizeHint), core::ir::MemoryDomain::MEMORY, sizeHint);
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
