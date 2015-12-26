/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "SPUDisassembler.h"

#include <nc/common/make_unique.h>

#include "SPUArchitecture.h"
#include "SPUInstruction.h"

namespace nc {
namespace arch {
namespace spu {

SPUDisassembler::SPUDisassembler(const SPUArchitecture *architecture):
    core::arch::Disassembler(architecture)
{
}

SPUDisassembler::~SPUDisassembler() {}

std::shared_ptr<core::arch::Instruction> SPUDisassembler::disassembleSingleInstruction(ByteAddr pc, const void *buffer, ByteSize size) {
    return std::make_shared<SPUInstruction>(pc, SPUInstruction::MAX_SIZE, buffer);
}

}}} // namespace nc::arch::spu

/* vim:set et sts=4 sw=4: */
