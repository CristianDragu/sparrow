#pragma once

#include "Nest/Api/TypeRef.h"

#include <boost/function.hpp>
#include <string>
#include <unordered_map>

FWD_CLASS1(Feather, Decl);
FWD_CLASS2(LLVMB,Tr, DebugInfo);

FWD_CLASS1(llvm, LLVMContext);
FWD_CLASS1(llvm, Module);
FWD_CLASS1(llvm, Function);

typedef struct Nest_Node Node;

namespace LLVMB
{
    /// Class that represents a backend module.
    /// It is responsible for the translation of an intermediate code into LLVM bitcode
    class Module
    {
    public:
        enum NodePropertyType
        {
            propValue = 0,
            propFunDecl,
            propTransType,
            propWhileInstr,
            propWhileStepLabel,
            propWhileEndLabel,
        };

        typedef boost::function<Node*(Node*)> NodeFun;

    public:
        explicit Module(const string& name);
        virtual ~Module();

        /// Returns true if we are generating code for CT, and false if we are generating code for RT
        virtual bool isCt() const = 0;

        /// Add a global constructor function
        virtual void addGlobalCtor(llvm::Function* fun) = 0;

        /// Add a global destructor function to this module
        virtual void addGlobalDtor(llvm::Function* fun) = 0;
        
        /// Getter for the functor used to translate nodes from CT to RT
        virtual NodeFun ctToRtTranslator() const = 0;

    public:
        /// Getter for the LLVM context used for the code generation
        llvm::LLVMContext& llvmContext() const { return *llvmContext_; }

        /// Getter for the LLVM module that is used for code generation
        llvm::Module& llvmModule() const { return *llvmModule_; }

        /// Getter for the debug information object used for this module
        Tr::DebugInfo* debugInfo() const { return debugInfo_; }

        /// Returns true if the given declaration makes sense in the current module
        bool canUse(Node* decl) const;

        /// Getter/setter for the llvm functions defined by this module
        bool isFunctionDefined(llvm::Function* fun) const;
        void addDefinedFunction(llvm::Function* fun);

        /// Getters/setter for node properties
        void** getNodePropertyPtr(Node* node, NodePropertyType type);
        void setNodeProperty(Node* node, NodePropertyType type, void* value);

    // Helpers
    public:
        template <typename T>
        T* getNodePropertyValue(Node* node, NodePropertyType type)
        {
            return reinterpret_cast<T*>(getNodePropertyPtr(node, type));
        }

    protected:
        typedef pair<Node*, NodePropertyType> PropKey;
        struct PropKeyHash
        {
            size_t operator()(const PropKey& k) const
            {
                return std::hash<Node*>()(k.first) + 393241*int(k.second);
            }
        };

        llvm::LLVMContext* llvmContext_;
        llvm::Module* llvmModule_;
        unordered_map<PropKey, void*, PropKeyHash> nodeProperties_;
        unordered_set<llvm::Function*> definedFunctions_;

        /// If set, it represents the object that is used to generate debug information for this module
        Tr::DebugInfo* debugInfo_;

    public:
        unordered_map<TypeRef, llvm::Type*> translatedTypes_;
    };

}
