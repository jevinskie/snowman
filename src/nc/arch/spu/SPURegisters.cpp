/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#include "SPURegisters.h"

namespace nc { namespace arch { namespace spu {

SPURegisters::SPURegisters() {
#define REGISTER_TABLE <nc/arch/spu/SPURegisterTable.i>
#include <nc/core/arch/RegistersConstructor.i>
}

}}} // namespace nc::arch::spu

/* vim:set et sts=4 sw=4: */
