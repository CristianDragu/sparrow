#pragma once

#include <Nest/Intermediate/TypeRef.h>

#include <unordered_map>

FWD_CLASS1(llvm, LLVMContext);
FWD_CLASS1(llvm, Module);

namespace LLVMB
{
    /// Class that is responsible for computing the sizeOf and alignOf data types
    class DataLayoutHelper
    {
    public:
        DataLayoutHelper();
        ~DataLayoutHelper();

        /// Get the size of the given type in bytes, according to the current data layout
        size_t getSizeOf(Nest::TypeRef type);

        /// Get the alignment of the given type in bytes, according to the current data layout
        size_t getAlignOf(Nest::TypeRef type);

    private:
        llvm::LLVMContext* llvmContext_;
        llvm::Module* llvmModule_;

        unordered_map<Nest::TypeRef, size_t> sizesOfTypes_;
        unordered_map<Nest::TypeRef, size_t> alignmentsOfTypes_;
    };

}
