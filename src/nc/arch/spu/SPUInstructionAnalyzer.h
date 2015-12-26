/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#pragma once

#include <nc/config.h>

#include <nc/core/irgen/InstructionAnalyzer.h>

namespace nc {
namespace arch {
namespace spu {

class SPUArchitecture;
class SPUInstructionAnalyzerImpl;

class SPUInstructionAnalyzer: public core::irgen::InstructionAnalyzer {
    std::unique_ptr<SPUInstructionAnalyzerImpl> impl_;

public:
    SPUInstructionAnalyzer(const SPUArchitecture *architecture);

    ~SPUInstructionAnalyzer();

protected:
    virtual void doCreateStatements(const core::arch::Instruction *instruction, core::ir::Program *program) override;
};

}}} // namespace nc::arch::spu

/* vim:set et sts=4 sw=4: */
