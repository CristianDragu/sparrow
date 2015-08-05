#pragma once

FWD_CLASS1(LLVMB, Module)
FWD_STRUCT1(Nest, Node);

FWD_CLASS1(llvm, Type);

namespace LLVMB { namespace Tr
{
    // Main translation method: translates a top-level intermediate node into a LLVM node
    void translateTopLevelNode(Nest::Node* node, Module& module);

    void translateBackendCode(Nest::Node* node, Module& module);
    llvm::Type* translateClass(Nest::Node* node, Module& module);
    llvm::Value* translateGlobalVar(Nest::Node* node, Module& module);
}}
