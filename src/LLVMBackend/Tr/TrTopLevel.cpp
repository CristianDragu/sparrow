#include <StdInc.h>
#include "TrTopLevel.h"
#include "TrFunction.h"
#include "TrType.h"
#include "TrLocal.h"
#include "Module.h"

#include <Nest/Intermediate/Type.h>
#include <Nest/Common/Diagnostic.h>
#include <Nest/Compiler.h>
#include <Nest/CompilerSettings.h>

#include <Feather/Nodes/Properties.h>
#include <Feather/Nodes/Decls/Class.h>
#include <Feather/Nodes/Decls/Function.h>
#include <Feather/Nodes/Decls/Var.h>
#include <Feather/Util/Decl.h>

#ifdef _MSC_VER
#pragma warning(push,1)
#endif
#include <llvm/Linker/Linker.h>
#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

using namespace LLVMB;
using namespace LLVMB::Tr;
using namespace Nest;
using namespace Feather;

namespace
{
    void translateNodeList(Node* node, Module& module)
    {
        for ( Node* child: node->children )
        {
            if ( child )
                translateTopLevelNode(child, module);
        }
    }

    void translateGlobalDestructAction(Node* node, Module& module)
    {
        llvm::Function* fun = makeFunThatCalls(node->children[0], module, "__global_dtor");
        if ( fun )
            module.addGlobalDtor(fun);
    }

    void translateGlobalConstructAction(Node* node, Module& module)
    {
        llvm::Function* fun = makeFunThatCalls(node->children[0], module, "__global_ctor");
        if ( fun )
            module.addGlobalCtor(fun);
    }
}



void Tr::translateTopLevelNode(Node* node, Module& module)
{
    // If this node is explained, then translate its explanation
    Node* expl = Nest::explanation(node);
    if ( node != expl )
    {
        translateTopLevelNode(expl, module);
    }
    else
    {
        // Depending on the type of the node, do a specific translation

        switch ( node->nodeKind - firstFeatherNodeKind )
        {
        case nkRelFeatherNop:                      break;
        case nkRelFeatherNodeList:                 translateNodeList(node, module); break;
        case nkRelFeatherBackendCode:              translateBackendCode(node, module); break;
        case nkRelFeatherGlobalDestructAction:     translateGlobalDestructAction(node, module); break;
        case nkRelFeatherGlobalConstructAction:    translateGlobalConstructAction(node, module); break;
        case nkRelFeatherDeclClass:                translateClass((Class*) node, module); break;
        case nkRelFeatherDeclFunction:             translateFunction((Function*) node, module); break;
        case nkRelFeatherDeclVar:                  translateGlobalVar((Var*) node, module); break;
        default:
            REP_ERROR(node->location, "Don't know how to interpret a node of this kind (%1%)") % Nest::nodeKindName(node);
        }
    }
}


void Tr::translateBackendCode(Node* node, Module& module)
{
    // Generate a new module from the given backend code
    llvm::SMDiagnostic error;
    const string& code = Nest::getCheckPropertyString(node, propCode);
    llvm::Module* resModule = llvm::ParseAssemblyString(code.c_str(), nullptr, error, module.llvmContext());
    if ( !resModule )
    {
        Location loc = node->location;
        if ( loc.startLineNo() == 1 && loc.startColNo() == 1 )
            loc = Location(*node->location.sourceCode(), error.getLineNo(), error.getColumnNo());
        REP_ERROR(loc, "Cannot parse backend code node: %1% (line: %2%)") % string(error.getMessage()) % error.getLineNo();
    }

    // Make sure it has the same data layout and target triple
    resModule->setDataLayout(module.llvmModule().getDataLayout());
    resModule->setTargetTriple(module.llvmModule().getTargetTriple());

    // Merge the new module into the current module
    string errMsg;
    llvm::Linker::LinkModules(&module.llvmModule(), resModule, llvm::Linker::DestroySource, &errMsg);
    if ( !errMsg.empty() )
        REP_ERROR(node->location, "Cannot merge backend code node into main LLVM module: %1%") % errMsg;
}

llvm::Type* Tr::translateClass(Feather::Class* node, Module& module)
{
    // Check for ct/non-ct compatibility
    if ( !module.canUse(node->node()) )
        return nullptr;

    if ( !node->type() || 0 == strcmp(node->type()->description, "BasicBlock") )
    {
        ASSERT(true);
    }

    // If this class was already translated, don't do it again
    llvm::Type** transType = module.getNodePropertyValue<llvm::Type*>(node->node(), Module::propTransType);
    if ( transType )
        return *transType;

    // Check if this is a standard/native type
    const string* nativeName = node->getPropertyString(propNativeName);
    if ( nativeName )
    {
        llvm::Type* t = getNativeLLVMType(node->location(), *nativeName, module.llvmContext());
        if ( t )
        {
            // Set the translation type for this item
            module.setNodeProperty(node->node(), Module::propTransType, boost::any(t));
            return t;
        }
    }

    // Get or create the type, and set it as a property (don't add any subtypes yet to avoid endless loops)
    llvm::StructType* t = nullptr;
    if ( nativeName )
        t = module.llvmModule().getTypeByName(*nativeName);    // Make sure we reuse the name
    if ( !t )
    {
        const string* description = node->getPropertyString(propDescription);
        // Create a new struct type, possible with another name
        t = llvm::StructType::create(module.llvmContext(), description ? *description : getName(node->node()));
    }
    module.setNodeProperty(node->node(), Module::propTransType, boost::any(static_cast<llvm::Type*>(t)));

    // Now add the subtypes
    vector<llvm::Type*> fieldTypes;
    fieldTypes.reserve(node->fields().size());
    for ( auto f: node->fields() )
    {
        Var* field = (Var*) f;
        fieldTypes.push_back(getLLVMType(field->type(), module));
    }
    if ( t->isOpaque() )
    {
        t->setBody(fieldTypes);
    }
    return t;
}

llvm::Value* Tr::translateGlobalVar(Feather::Var* node, Module& module)
{
    // Check for ct/non-ct compatibility
    if ( !module.canUse(node->node()) )
        return nullptr;

    if ( node->nodeKind() != nkFeatherDeclVar )
        REP_ERROR(node->location(), "Invalid global variable %1%") % getName(node->node());

    // If we already translated this variable, make sure not to translate it again
    llvm::Value* val = getValue(module, *node->node(), false);
    if ( val )
        return val;

    llvm::Type* t = getLLVMType(node->type(), module);

    // Check if the variable has been declared before; if not, create it
    llvm::GlobalVariable* var = nullptr;
    const string* nativeName = node->getPropertyString(propNativeName);
    if ( nativeName )
        var = module.llvmModule().getGlobalVariable(*nativeName);
    if ( !var )
    {
        var = new llvm::GlobalVariable(
            module.llvmModule(),
            t,
            false, // isConstant
            llvm::GlobalValue::ExternalLinkage, // linkage
            0, // initializer - specified below
            getName(node->node())
            );
    }

    // Create a zero initializer for the variable
    if ( t->isIntegerTy() )
    {
        var->setInitializer(llvm::ConstantInt::get(static_cast<llvm::IntegerType*>(t), 0));
    }
    else if ( t->isFloatingPointTy() )
    {
        var->setInitializer(llvm::ConstantFP::get(t, 0.0));
    }
    else if ( t->isPointerTy() )
    {
        var->setInitializer(llvm::ConstantPointerNull::get(static_cast<llvm::PointerType*>(t)));
    }
    else if ( t->isStructTy() )
    {
        var->setInitializer(llvm::ConstantAggregateZero::get(t));
    }
    else
    {
        REP_ERROR(node->location(), "Don't know how to create zero initializer for the variable of type %1%")
            % node->type();
    }

    // Set the value for the variable
    setValue(module, *node->node(), var);
    return var;
}
