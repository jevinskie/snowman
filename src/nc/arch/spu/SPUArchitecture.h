/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#pragma once

#include <nc/config.h>

#include <nc/core/arch/Architecture.h>

namespace nc {
namespace arch {
namespace spu {

/**
 * 32-bit ARM Architecture.
 */
class SPUArchitecture: public nc::core::arch::Architecture {
    ByteOrder byteOrder_;

public:
    /**
     * Constructor.
     *
     * \param byteOrder Byte order of the main memory.
     */
    explicit
    SPUArchitecture(ByteOrder byteOrder);

    virtual ~SPUArchitecture();

    /**
     * \return Byte order of the main memory.
     */
    ByteOrder byteOrder() const { return byteOrder_; }

    ByteOrder getByteOrder(core::ir::Domain domain) const override;
    std::unique_ptr<core::arch::Disassembler> createDisassembler() const override;
    std::unique_ptr<core::irgen::InstructionAnalyzer> createInstructionAnalyzer() const override;
};

}}} // namespace nc::arch::spu

/* vim:set et sts=4 sw=4: */
