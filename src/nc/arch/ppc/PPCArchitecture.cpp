/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "PPCArchitecture.h"

#include <nc/common/make_unique.h>

#include <nc/core/MasterAnalyzer.h>

#include "PPCDisassembler.h"
#include "PPCInstruction.h"
#include "PPCInstructionAnalyzer.h"
#include "PPCRegisters.h"
#include "PPCCallingConventions.h"

namespace nc {
namespace arch {
namespace ppc {

PPCArchitecture::PPCArchitecture(ByteOrder byteOrder):
    byteOrder_(byteOrder)
{
    if (byteOrder == ByteOrder::LittleEndian) {
        setName(QLatin1String("ppc-le"));
    } else {
        setName(QLatin1String("ppc-be"));
    }
    setBitness(64);
    setMaxInstructionSize(PPCInstruction::maxSize());

    setRegisters(PPCRegisters::instance());

    static core::MasterAnalyzer masterAnalyzer;
    setMasterAnalyzer(&masterAnalyzer);

    addCallingConvention(std::make_unique<DefaultCallingConvention>());
}

PPCArchitecture::~PPCArchitecture() {}

ByteOrder PPCArchitecture::getByteOrder(core::ir::Domain domain) const {
    if (domain == core::ir::MemoryDomain::MEMORY ||
        domain == core::ir::MemoryDomain::STACK)
    {
        return byteOrder_;
    } else {
        return ByteOrder::LittleEndian;
    }
}

std::unique_ptr<core::arch::Disassembler> PPCArchitecture::createDisassembler() const {
    return std::make_unique<PPCDisassembler>(this);
}

std::unique_ptr<core::irgen::InstructionAnalyzer> PPCArchitecture::createInstructionAnalyzer() const {
    return std::make_unique<PPCInstructionAnalyzer>(this);
}

}}} // namespace nc::arch::ppc

/* vim:set et sts=4 sw=4: */
