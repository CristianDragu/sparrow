#pragma once

#include "Nest/Utils/NodeUtils.h"

inline NodeRange all(NodeArray nodes) {
    NodeRange res = {nodes.beginPtr, nodes.endPtr};
    return res;
}

inline Node** begin(NodeRange r) { return r.beginPtr; }
inline Node** end(NodeRange r) { return r.endPtr; }

inline Node** begin(NodeArray r) { return r.beginPtr; }
inline Node** end(NodeArray r) { return r.endPtr; }

/// Construct a NodeRange from an initializer_list
inline NodeRange fromIniList(std::initializer_list<Node*> l) {
    NodeRange res = {(Node**)l.begin(), (Node**)l.end()};
    return res;
}

inline Node*& at(NodeRange r, unsigned int idx) { return r.beginPtr[idx]; }

inline Node*& at(NodeArray arr, unsigned int idx) { return arr.beginPtr[idx]; }

inline unsigned int size(NodeRange r) { return Nest_nodeRangeSize(r); }
inline unsigned int size(NodeArray arr) { return Nest_nodeArraySize(arr); }

//! Class that destructs a node array on its destructor.
//! A scoped action for deleting the array
class NodeArrayDestroyer {
    NodeArray& arr;

public:
    NodeArrayDestroyer(NodeArray& a)
        : arr(a) {}
    ~NodeArrayDestroyer() { Nest_freeNodeArray(arr); }

    NodeArrayDestroyer(const NodeArrayDestroyer&) = delete;
    NodeArrayDestroyer(NodeArrayDestroyer&&) = delete;
    const NodeArrayDestroyer& operator=(const NodeArrayDestroyer&) = delete;
    const NodeArrayDestroyer& operator=(NodeArrayDestroyer&&) = delete;
};
