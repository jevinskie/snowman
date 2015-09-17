/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "PPCDisassembler.h"

#include <nc/common/make_unique.h>

#include "PPCArchitecture.h"
#include "PPCInstruction.h"

namespace nc {
namespace arch {
namespace ppc {

PPCDisassembler::PPCDisassembler(const PPCArchitecture *architecture):
    core::arch::Disassembler(architecture)
{
    mode_ = CS_MODE_64;
    if (architecture->byteOrder() == ByteOrder::LittleEndian) {
        mode_ |= CS_MODE_LITTLE_ENDIAN;
    } else if (architecture->byteOrder() == ByteOrder::BigEndian) {
        mode_ |= CS_MODE_BIG_ENDIAN;
    }
    capstone_ = std::make_unique<core::arch::Capstone>(CS_ARCH_PPC, mode_);
}

PPCDisassembler::~PPCDisassembler() {}

std::shared_ptr<core::arch::Instruction> PPCDisassembler::disassembleSingleInstruction(ByteAddr pc, const void *buffer, ByteSize size) {
    if (auto instr = capstone_->disassemble(pc, buffer, size, 1)) {
        /* Instructions must be aligned to their size. */
        if ((instr->address & (instr->size - 1)) == 0) {
            return std::make_shared<PPCInstruction>(mode_, instr->address, instr->size, buffer);
        }
    }
    return nullptr;
}

}}} // namespace nc::arch::arm

/* vim:set et sts=4 sw=4: */
