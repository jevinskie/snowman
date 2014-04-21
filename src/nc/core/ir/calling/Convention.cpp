/* The file is part of Snowman decompiler.             */
/* See doc/licenses.txt for the licensing information. */

//
// SmartDec decompiler - SmartDec is a native code to C/C++ decompiler
// Copyright (C) 2015 Alexander Chernov, Katerina Troshina, Yegor Derevenets,
// Alexander Fokin, Sergey Levin, Leonid Tsvetkov
//
// This file is part of SmartDec decompiler.
//
// SmartDec decompiler is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SmartDec decompiler is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SmartDec decompiler.  If not, see <http://www.gnu.org/licenses/>.
//

#include "Convention.h"

#include <nc/common/Foreach.h>
#include <nc/common/Range.h>

#include <nc/core/ir/Statement.h>
#include <nc/core/ir/Term.h>

namespace nc {
namespace core {
namespace ir {
namespace calling {

Convention::Convention(QString name):
    name_(std::move(name)),
    firstArgumentOffset_(0),
    argumentAlignment_(0),
    calleeCleanup_(false)
{}

Convention::~Convention() {}

namespace {

/**
 * Rounds given number down to the closest multiple of the multiple.
 */
template<class T>
T roundDown(T number, T multiple) {
    assert(multiple > 0);

    auto remainder = number % multiple;
    if (remainder == 0) {
        return number;
    }

    if (number >= 0) {
        assert(remainder > 0);
        return number - remainder;
    } else {
        assert(remainder < 0);
        return number - remainder - multiple;
    }
}

/**
 * Rounds given number up to the closest multiple of the multiple.
 */
template<class T>
T roundUp(T number, T multiple) {
    assert(multiple > 0);

    auto remainder = number % multiple;
    if (remainder == 0) {
        return number;
    }

    if (number >= 0) {
        assert(remainder > 0);
        return number - remainder + multiple;
    } else {
        assert(remainder < 0);
        return number - remainder;
    }
}

#ifndef NDEBUG
bool testRounding() {
    assert(roundDown(3, 4) == 0);
    assert(roundDown(5, 4) == 4);
    assert(roundDown(-3, 4) == -4);
    assert(roundDown(-5, 4) == -8);

    assert(roundUp(3, 4) == 4);
    assert(roundUp(5, 4) == 8);
    assert(roundUp(-3, 4) == 0);
    assert(roundUp(-5, 4) == -4);

    return true;
}

const bool roundingWorks = testRounding();
#endif

} // anonymous namespace

MemoryLocation Convention::getArgumentLocationCovering(const MemoryLocation &memoryLocation) const {
    if (!memoryLocation) {
        return MemoryLocation();
    }

    /* Note: this assumes the stack growing down. */
    if (memoryLocation.domain() == MemoryDomain::STACK &&
        memoryLocation.addr() >= firstArgumentOffset()
    ) {
        /* Align the location properly. */
        if (argumentAlignment()) {
            auto addr = roundDown(memoryLocation.addr(), argumentAlignment());
            auto endAddr = roundUp(memoryLocation.endAddr(), argumentAlignment());
            return MemoryLocation(MemoryDomain::STACK, addr, endAddr - addr);
        } else {
            return memoryLocation;
        }
    }

    foreach (const auto &argumentLocation, argumentLocations()) {
        if (argumentLocation.covers(memoryLocation)) {
            return argumentLocation;
        }
    }

    return MemoryLocation();
}

std::vector<MemoryLocation> Convention::sortArguments(std::vector<MemoryLocation> arguments) const {
    std::vector<MemoryLocation> result;
    result.reserve(arguments.size());

    /* Copy non-stack arguments in the order. */
    foreach (const auto &argumentLocation, argumentLocations()) {
        auto predicate = [&argumentLocation](const MemoryLocation &memoryLocation) {
            return argumentLocation.covers(memoryLocation);
        };

        std::copy_if(arguments.begin(), arguments.end(), std::back_inserter(result), predicate);
        arguments.erase(std::remove_if(arguments.begin(), arguments.end(), predicate), arguments.end());
    }

    /* What is left are stack arguments. Sort and add them. */
    std::sort(arguments.begin(), arguments.end());
    result.insert(result.end(), arguments.begin(), arguments.end());

    return result;
}

void Convention::addArgumentLocation(const MemoryLocation &memoryLocation) {
    assert(memoryLocation);
    assert(!nc::contains(argumentLocations_, memoryLocation));

    argumentLocations_.push_back(memoryLocation);
}

void Convention::addReturnValueTerm(std::unique_ptr<Term> term) {
    assert(term != NULL);
    returnValueTerms_.push_back(std::move(term));
}

void Convention::addEnterStatement(std::unique_ptr<Statement> statement) {
    assert(statement != NULL);
    entryStatements_.push_back(std::move(statement));
}

} // namespace calling
} // namespace ir
} // namespace core
} // namespace nc

/* vim:set et sts=4 sw=4: */