/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#pragma once

#include <nc/config.h>

#include <nc/core/irgen/InstructionAnalyzer.h>

namespace nc {
namespace arch {
namespace ppc {

class PPCArchitecture;
class PPCInstructionAnalyzerImpl;

class PPCInstructionAnalyzer: public core::irgen::InstructionAnalyzer {
    std::unique_ptr<PPCInstructionAnalyzerImpl> impl_;

public:
    PPCInstructionAnalyzer(const PPCArchitecture *architecture);

    ~PPCInstructionAnalyzer();

protected:
    virtual void doCreateStatements(const core::arch::Instruction *instruction, core::ir::Program *program) override;
};

}}} // namespace nc::arch::ppc

/* vim:set et sts=4 sw=4: */
