#pragma once

FWD_CLASS1(Nest, Type)
FWD_CLASS1(Nest, Location)
FWD_CLASS1(Feather, Function)
FWD_CLASS1(LLVMB, Module)

FWD_CLASS1(llvm, Type);
FWD_CLASS1(llvm, LLVMContext);

namespace LLVMB { namespace Tr
{
    /// Translates the given type into a LLVM type
    llvm::Type* getLLVMType(Nest::Type* type, Module& module);

    /// Get the LLVM type for a native type
    /// If this is not a LLVM native type, it will return null
    llvm::Type* getNativeLLVMType(const Nest::Location& loc, const string& nativeName, llvm::LLVMContext& llvmContext);

    /// Gets the LLVM type corresponding to the given function declaration
    llvm::Type* getLLVMFunctionType(Feather::Function* funDecl, int ignoreArg, Module& module);
}}
