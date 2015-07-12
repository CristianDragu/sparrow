#pragma once

#include "TypeRef.h"
#include "EvalMode.h"

#include <vector>

FWD_STRUCT1(Nest, Node)
FWD_CLASS3(Nest,Common,Ser, OutArchive)
FWD_CLASS3(Nest,Common,Ser, InArchive)

namespace Nest
{
    /// Represents a type
    struct Type
    {
        unsigned typeKind : 8;            ///< The type ID
        EvalMode mode : 8;              ///< The evaluation mode of this type
        unsigned numSubtypes : 16;      ///< The number of subtypes of this type
        unsigned numReferences : 16;    ///< The number of references of this type
        unsigned hasStorage : 1;        ///< True if this is type with storage
        unsigned canBeUsedAtCt : 1;     ///< Can we use this type at CT?
        unsigned canBeUsedAtRt : 1;     ///< Can we use this type at RT?
        unsigned flags : 32;            ///< Additional flags

        /// The subtypes of this type
        TypeRef* subTypes;

        /// Optional, the node that introduces this type
        Node* referredNode;

        /// The description of the type -- mainly used for debugging purposes
        const char* description;
    };

    /// Equality comparison, by content
    bool operator ==(const Type& lhs, const Type& rhs);

    /// Get a stock type that matches the settings from the reference
    /// We guarantee that the same types will have the same pointers
    /// If we cannot find a type that matches the reference, this will return null
    TypeRef findStockType(const Type& reference);

    /// Insert a new type into our stock
    /// Returns the pointer to be used to represent this new type
    TypeRef insertStockType(const Type& newType);


    template <typename T>
    basic_ostream<T>& operator << (basic_ostream<T>& os, TypeRef t)
    {
        if ( t )
            os << t->description;
        else
            os << "<null-type>";
        return os;
    }

    void save(const Type& obj, Common::Ser::OutArchive& ar);
    void load(Type& obj, Common::Ser::InArchive& ar);

    /// Some common operations on types

    /// Function that changes the mode for the given type
    TypeRef changeTypeMode(TypeRef type, EvalMode newMode);
}
