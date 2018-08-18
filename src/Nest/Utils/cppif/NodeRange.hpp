#pragma once

#include "Nest/Api/NodeRange.h"
#include "Nest/Utils/cppif/NodeHandle.hpp"

namespace Nest {

using Node = Nest_Node;

/**
 * @brief   A range of nodes, contiguously aligned in memory.
 *
 * This essentially represents a subset of an array of node pointers.
 * The nodes pointed by this range are mutable. However the actual pointers to the node are not.
 *
 * @see     NodeRangeM
 */
struct NodeRange {
    Nest_NodeRange range;

    //! Construct a from Nest_NodeRange data structure
    NodeRange(Nest_NodeRange r)
        : range(r) {}

    NodeRange(Nest_NodeRangeM r)
        : range{r.beginPtr, r.endPtr} {}

    //! Construct from an initializer list
    explicit NodeRange(std::initializer_list<Node*> l)
        : range{l.begin(), l.end()} {}

    //! Automatic conversion to Nest_NodeRange
    operator Nest_NodeRange() const { return range; }

    //! Returns true if there are no nodes in this range
    bool empty() const { return range.beginPtr == range.endPtr; }

    //! Gets the number of nodes in this range
    int size() const { return Nest_nodeRangeSize(range); }
    //! Returns the node handle at the given index
    NodeHandle operator[](int idx) const {
        ASSERT(idx < size());
        return range.beginPtr[idx];
    }

    //! Skip the given number of elements and return the remaining range
    NodeRange skip(int num) const {
        ASSERT(num>=0);
        ASSERT(num <= size());
        return Nest_NodeRange{range.beginPtr+num, range.endPtr};
    }

    const NodeHandle* begin() const {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<const NodeHandle*>(range.beginPtr);
    }
    const NodeHandle* end() const {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<const NodeHandle*>(range.endPtr);
    }

    const char* toString() const;
};

/**
 * @brief   A range of nodes, contiguously aligned in memory -- with mutability.
 *
 * This essentially represents a subset of an array of node pointers.
 * The nodes pointers and the nodes contents are all mutable.
 *
 * @see     NodeRange
 */
struct NodeRangeM {
    Nest_NodeRangeM range;

    //! Construct a from Nest_NodeRangeM data structure
    NodeRangeM(Nest_NodeRangeM r)
        : range(r) {}

    //! Automatic conversion to Nest_NodeRangeM
    operator Nest_NodeRangeM() const { return range; }

    //! Automatic conversion to NodeRange
    operator NodeRange() const { return Nest_NodeRange{range.beginPtr, range.endPtr}; }

    //! Returns true if there are no nodes in this range
    bool empty() const { return range.beginPtr == range.endPtr; }

    //! Gets the number of nodes in this range
    int size() const { return (int) Nest_nodeRangeMSize(range); }
    //! Returns the node handle at the given index
    NodeHandle& operator[](int idx) const {
        ASSERT(idx < size());
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return *reinterpret_cast<NodeHandle*>(&range.beginPtr[idx]);
    }

    const NodeHandle* begin() const {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<const NodeHandle*>(range.beginPtr);
    }
    const NodeHandle* end() const {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<const NodeHandle*>(range.endPtr);
    }

    const char* toString() const;
};

ostream& operator<<(ostream& os, NodeRange r);


} // namespace Nest
