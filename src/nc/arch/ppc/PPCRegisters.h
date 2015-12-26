/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#pragma once

#include <nc/config.h>

#include <nc/core/arch/Registers.h>

namespace nc { namespace arch { namespace ppc {

/**
 * Container class for PPC registers.
 */
class PPCRegisters: public core::arch::StaticRegisters<PPCRegisters> {
public:
    PPCRegisters();

#define REGISTER_TABLE <nc/arch/ppc/PPCRegisterTable.i>
#include <nc/core/arch/Registers.i>
};

}}} // namespace nc::arch::ppc

/* vim:set et sts=4 sw=4: */
