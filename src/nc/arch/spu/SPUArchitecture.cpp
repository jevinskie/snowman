/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "SPUArchitecture.h"

#include <nc/common/make_unique.h>

#include <nc/core/MasterAnalyzer.h>

#include "SPUDisassembler.h"
#include "SPUInstruction.h"
#include "SPUInstructionAnalyzer.h"
#include "SPURegisters.h"
#include "SPUCallingConventions.h"

namespace nc {
namespace arch {
namespace spu {

SPUArchitecture::SPUArchitecture(ByteOrder byteOrder):
    byteOrder_(byteOrder)
{
    setName(QLatin1String("spu-be"));
    setBitness(64);
    setMaxInstructionSize(SPUInstruction::MAX_SIZE);

    setRegisters(SPURegisters::instance());

    static core::MasterAnalyzer masterAnalyzer;
    setMasterAnalyzer(&masterAnalyzer);

    addCallingConvention(std::make_unique<DefaultCallingConvention>());
}

SPUArchitecture::~SPUArchitecture() {}

ByteOrder SPUArchitecture::getByteOrder(core::ir::Domain domain) const {
    return ByteOrder::BigEndian;
}

std::unique_ptr<core::arch::Disassembler> SPUArchitecture::createDisassembler() const {
    return std::make_unique<SPUDisassembler>(this);
}

std::unique_ptr<core::irgen::InstructionAnalyzer> SPUArchitecture::createInstructionAnalyzer() const {
    return std::make_unique<SPUInstructionAnalyzer>(this);
}

}}} // namespace nc::arch::spu

/* vim:set et sts=4 sw=4: */
