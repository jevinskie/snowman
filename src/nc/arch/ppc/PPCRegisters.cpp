/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "PPCRegisters.h"

namespace nc { namespace arch { namespace ppc {

PPCRegisters::PPCRegisters() {
#define REGISTER_TABLE <nc/arch/ppc/PPCRegisterTable.i>
#include <nc/core/arch/RegistersConstructor.i>
}

}}} // namespace nc::arch::ppc

/* vim:set et sts=4 sw=4: */
