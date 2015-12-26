/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#pragma once

#include <nc/config.h>

#include <memory>

#include <nc/core/arch/Disassembler.h>

namespace nc {
namespace arch {
namespace spu {

class SPUArchitecture;

/**
 * Disassembler for SPU architecture.
 */
class SPUDisassembler: public core::arch::Disassembler {
    int mode_;

public:
    /**
     * \param architecture Valid pointer to the SPU architecture.
     */
    SPUDisassembler(const SPUArchitecture *architecture);

    virtual ~SPUDisassembler();

    std::shared_ptr<core::arch::Instruction> disassembleSingleInstruction(ByteAddr pc, const void *buffer, ByteSize size) override;
};

}}} // namespace nc::arch::spu

/* vim:set et sts=4 sw=4: */
