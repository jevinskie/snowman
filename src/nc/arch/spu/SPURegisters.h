/* The file is part of Snowman decompiler. */
/* See doc/licenses.asciidoc for the licensing information. */

#pragma once

#include <nc/config.h>

#include <nc/core/arch/Registers.h>

namespace nc { namespace arch { namespace spu {

/**
 * Container class for SPU registers.
 */
class SPURegisters: public core::arch::StaticRegisters<SPURegisters> {
public:
    SPURegisters();

#define REGISTER_TABLE <nc/arch/spu/SPURegisterTable.i>
#include <nc/core/arch/Registers.i>
};

}}} // namespace nc::arch::spu

/* vim:set et sts=4 sw=4: */
