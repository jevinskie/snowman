/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#pragma once

#include <nc/config.h>

#include <nc/core/arch/Architecture.h>

namespace nc {
namespace arch {
namespace ppc {

class PPCArchitecture: public nc::core::arch::Architecture {
    ByteOrder byteOrder_;

public:
    /**
     * Constructor.
     *
     * \param byteOrder Byte order of the main memory.
     */
    explicit
    PPCArchitecture(ByteOrder byteOrder);

    virtual ~PPCArchitecture();

    /**
     * \return Byte order of the main memory.
     */
    ByteOrder byteOrder() const { return byteOrder_; }

    ByteOrder getByteOrder(core::ir::Domain domain) const override;
    std::unique_ptr<core::arch::Disassembler> createDisassembler() const override;
    std::unique_ptr<core::irgen::InstructionAnalyzer> createInstructionAnalyzer() const override;
};

}}} // namespace nc::arch::ppc

/* vim:set et sts=4 sw=4: */
