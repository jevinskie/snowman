/* The file is part of Snowman decompiler.             */
/* See doc/licenses.txt for the licensing information. */

/* * SmartDec decompiler - SmartDec is a native code to C/C++ decompiler
 * Copyright (C) 2015 Alexander Chernov, Katerina Troshina, Yegor Derevenets,
 * Alexander Fokin, Sergey Levin, Leonid Tsvetkov
 *
 * This file is part of SmartDec decompiler.
 *
 * SmartDec decompiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SmartDec decompiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SmartDec decompiler.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <nc/config.h>

#include <memory>

#include <QString>

#include <nc/common/ilist.h>

#include <nc/core/ir/MemoryLocation.h>

namespace nc {
namespace core {
namespace ir {

class Statement;
class Term;

namespace calling {

/**
 * Description of a calling convention.
 */
class Convention {
public:
    typedef nc::ilist<Statement> Statements;

private:
    QString name_; ///< Name of the calling convention.

    MemoryLocation stackPointer_; ///< Memory location of stack pointer register.

    BitSize firstArgumentOffset_; ///< Offset of the first argument in a function's stack frame.
    BitSize argumentAlignment_; ///< Alignment of stack arguments in bits.

    std::vector<MemoryLocation> argumentLocations_; ///< Possible locations of arguments.
    std::vector<std::unique_ptr<const Term>> returnValueTerms_; ///< Terms denoting where return values may be kept.

    bool calleeCleanup_; ///< Callee cleans up arguments.

    nc::ilist<Statement> entryStatements_; ///< Statements executed when the function is entered.

public:
    /**
     * Constructor.
     *
     * \param name Name of the calling convention.
     */
    Convention(QString name);

    /**
     * Destructor.
     */
    ~Convention();

    /**
     * \return Name of the calling convention.
     */
    const QString &name() const { return name_; }

    /**
     * \return MemoryLocation of stack pointer register.
     */
    const MemoryLocation &stackPointer() const { return stackPointer_; }

    /**
     * \return Offset of the first argument in a function's stack frame.
     */
    BitSize firstArgumentOffset() const { return firstArgumentOffset_; }

    /**
     * Sets the offset of the first argument in a function's stack frame.
     *
     * \param firstArgumentOffset The offset.
     */
    void setFirstArgumentOffset(BitSize firstArgumentOffset) { firstArgumentOffset_ = firstArgumentOffset; }

    /**
     * \return Alignment of stack arguments in bits.
     */
    BitSize argumentAlignment() const { return argumentAlignment_; }

    /**
     * \return List of possible argument locations.
     */
    const std::vector<MemoryLocation> &argumentLocations() const { return argumentLocations_; }

    /**
     * \param memoryLocation A memory location.
     *
     * \return Possible argument location covering given memory location.
     *         If none is found, an invalid memory location is returned.
     */
    MemoryLocation getArgumentLocationCovering(const MemoryLocation &memoryLocation) const;

    /**
     * Sorts the argument locations in the way they are described in the convention.
     *
     * \param arguments List of memory locations.
     *
     * \return Sorted list of memory locations.
     */
    std::vector<MemoryLocation> sortArguments(std::vector<MemoryLocation> arguments) const;

    /**
     * \return List of terms where return values can be kept.
     */
    const std::vector<const Term *> &returnValueTerms() const {
        return reinterpret_cast<const std::vector<const Term *> &>(returnValueTerms_);
    }

    /**
     * \return True if callee cleans up arguments.
     */
    bool calleeCleanup() const { return calleeCleanup_; }
    
    /**
     * Statements executed when a function is entered.
     *
     * Such statements are typically used for setting various flags to the values
     * they are guaranteed to be set by calling convention, i.e. set Intel's direction
     * flag to zero.
     */
    const Statements &entryStatements() const { return entryStatements_; }

protected:
    /**
     * Sets MemoryLocation of stack pointer register.
     *
     * \param[in] location MemoryLocation.
     */
    void setStackPointer(const MemoryLocation &location) { stackPointer_ = location; }

    /**
     * Sets alignment of stack arguments.
     *
     * \param[in] argumentAlignment Alignment in bits.
     */
    void setArgumentAlignment(BitSize argumentAlignment) { argumentAlignment_ = argumentAlignment; };

    /**
     * Adds a possible argument location.
     *
     * \param[in] memoryLocation Valid memory location.
     */
    void addArgumentLocation(const MemoryLocation &memoryLocation);

    /**
     * Adds a term in which return values may be kept.
     *
     * \param term Valid pointer to the term.
     */
    void addReturnValueTerm(std::unique_ptr<Term> term);

    /**
     * Sets whether callee cleans up arguments.
     *
     * \param[in] calleeCleanup Whether callee cleans up arguments.
     */
    void setCalleeCleanup(bool calleeCleanup) { calleeCleanup_ = calleeCleanup; }

    /**
     * Adds a statement executed when a function is entered.
     *
     * \param statement Valid pointer to a statement.
     */
    void addEnterStatement(std::unique_ptr<Statement> statement);
};

} // namespace calling
} // namespace ir
} // namespace core
} // namespace nc

/* vim:set et sts=4 sw=4: */