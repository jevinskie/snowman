/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#pragma once

#include <nc/config.h>

#include <memory>

#include <nc/core/arch/Capstone.h>
#include <nc/core/arch/Disassembler.h>

namespace nc {
namespace arch {
namespace ppc {

class PPCArchitecture;

/**
 * Disassembler for PPC architecture.
 */
class PPCDisassembler: public core::arch::Disassembler {
    std::unique_ptr<core::arch::Capstone> capstone_;
    int mode_;

public:
    /**
     * \param architecture Valid pointer to the PPC architecture.
     */
    PPCDisassembler(const PPCArchitecture *architecture);

    virtual ~PPCDisassembler();

    std::shared_ptr<core::arch::Instruction> disassembleSingleInstruction(ByteAddr pc, const void *buffer, ByteSize size) override;
};

}}} // namespace nc::arch::ppc

/* vim:set et sts=4 sw=4: */
