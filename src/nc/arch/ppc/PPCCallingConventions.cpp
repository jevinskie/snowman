/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "PPCCallingConventions.h"

#include <nc/common/make_unique.h>
#include <nc/core/ir/Statements.h>
#include <nc/core/ir/Terms.h>

#include "PPCArchitecture.h"
#include "PPCRegisters.h"

namespace nc {
namespace arch {
namespace ppc {

DefaultCallingConvention::DefaultCallingConvention():
    core::ir::calling::Convention(QLatin1String("Default"))
{
    setStackPointer(PPCRegisters::r1()->memoryLocation());

    setFirstArgumentOffset(0);
    setArgumentAlignment(64);

    std::vector<core::ir::MemoryLocation> rArgs;
    rArgs.push_back(PPCRegisters::r3()->memoryLocation());
    rArgs.push_back(PPCRegisters::r4()->memoryLocation());
    rArgs.push_back(PPCRegisters::r5()->memoryLocation());
    rArgs.push_back(PPCRegisters::r6()->memoryLocation());
    rArgs.push_back(PPCRegisters::r7()->memoryLocation());
    rArgs.push_back(PPCRegisters::r8()->memoryLocation());
    rArgs.push_back(PPCRegisters::r9()->memoryLocation());
    rArgs.push_back(PPCRegisters::r10()->memoryLocation());
    addArgumentGroup(std::move(rArgs));

    std::vector<core::ir::MemoryLocation> fArgs;
    fArgs.push_back(PPCRegisters::f1()->memoryLocation());
    fArgs.push_back(PPCRegisters::f2()->memoryLocation());
    fArgs.push_back(PPCRegisters::f3()->memoryLocation());
    fArgs.push_back(PPCRegisters::f4()->memoryLocation());
    fArgs.push_back(PPCRegisters::f5()->memoryLocation());
    fArgs.push_back(PPCRegisters::f6()->memoryLocation());
    fArgs.push_back(PPCRegisters::f7()->memoryLocation());
    fArgs.push_back(PPCRegisters::r8()->memoryLocation());
    fArgs.push_back(PPCRegisters::f9()->memoryLocation());
    fArgs.push_back(PPCRegisters::f10()->memoryLocation());
    fArgs.push_back(PPCRegisters::f11()->memoryLocation());
    fArgs.push_back(PPCRegisters::f12()->memoryLocation());
    fArgs.push_back(PPCRegisters::f13()->memoryLocation());
    addArgumentGroup(std::move(fArgs));

    addReturnValueLocation(PPCRegisters::r3()->memoryLocation());
    addReturnValueLocation(PPCRegisters::f1()->memoryLocation());

    addEnterStatement(std::make_unique<core::ir::Assignment>(
        std::make_unique<core::ir::MemoryLocationAccess>(PPCRegisters::lr()->memoryLocation()),
        std::make_unique<core::ir::Intrinsic>(core::ir::Intrinsic::RETURN_ADDRESS, PPCRegisters::lr()->size())
    ));
}

}}} // namespace nc::arch::arm

/* vim:set et sts=4 sw=4: */
