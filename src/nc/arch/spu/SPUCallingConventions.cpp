/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "SPUCallingConventions.h"

#include <nc/common/make_unique.h>
#include <nc/core/ir/Statements.h>
#include <nc/core/ir/Terms.h>

#include "SPUArchitecture.h"
#include "SPURegisters.h"

namespace nc {
namespace arch {
namespace spu {

DefaultCallingConvention::DefaultCallingConvention():
    core::ir::calling::Convention(QLatin1String("Default"))
{
    setStackPointer(SPURegisters::r1()->memoryLocation());

    setFirstArgumentOffset(0);
    setArgumentAlignment(128);

    std::vector<core::ir::MemoryLocation> args;
    args.push_back(SPURegisters::r3()->memoryLocation());
    args.push_back(SPURegisters::r4()->memoryLocation());
    args.push_back(SPURegisters::r5()->memoryLocation());
    args.push_back(SPURegisters::r6()->memoryLocation());
    args.push_back(SPURegisters::r7()->memoryLocation());
    args.push_back(SPURegisters::r8()->memoryLocation());
    args.push_back(SPURegisters::r9()->memoryLocation());
    args.push_back(SPURegisters::r10()->memoryLocation());
    args.push_back(SPURegisters::r11()->memoryLocation());
    args.push_back(SPURegisters::r12()->memoryLocation());
    args.push_back(SPURegisters::r13()->memoryLocation());
    args.push_back(SPURegisters::r14()->memoryLocation());
    args.push_back(SPURegisters::r15()->memoryLocation());
    addArgumentGroup(std::move(args));

    addReturnValueLocation(SPURegisters::r3()->memoryLocation());

    addEnterStatement(std::make_unique<core::ir::Assignment>(
        std::make_unique<core::ir::MemoryLocationAccess>(SPURegisters::r0()->memoryLocation()),
        std::make_unique<core::ir::Intrinsic>(core::ir::Intrinsic::RETURN_ADDRESS, SPURegisters::r0()->size())
    ));
}

}}} // namespace nc::arch::spu

/* vim:set et sts=4 sw=4: */
