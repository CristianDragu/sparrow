#include <StdInc.h>
#include "SprConcept.h"
#include "InstantiationsSet.h"
#include <NodeCommonsCpp.h>
#include <Helpers/SprTypeTraits.h>
#include <Helpers/DeclsHelpers.h>
#include <SparrowFrontendTypes.h>

#include <Feather/Nodes/FeatherNodes.h>
#include <Feather/Util/Decl.h>
#include <Feather/Util/TypeTraits.h>

using namespace SprFrontend;
using namespace Feather;

SprConcept::SprConcept(const Location& loc, string name, string paramName, DynNode* baseConcept, DynNode* ifClause, AccessType accessType)
    : DynNode(classNodeKind(), loc, {baseConcept, ifClause, nullptr})
{
    setName(this, move(name));
    setAccessType(this, accessType);
    setProperty("spr.paramName", move(paramName));
}

bool SprConcept::isFulfilled(TypeRef type)
{
    InstantiationsSet* instantiationsSet = children_[2]->as<InstantiationsSet>();

    if ( !isSemanticallyChecked() || !instantiationsSet )
        REP_INTERNAL(location_, "Invalid concept");

    DynNode* typeValue = createTypeNode(context_, location_, type);
    typeValue->semanticCheck();

    return nullptr != instantiationsSet->canInstantiate({typeValue}, context_->evalMode());
}

TypeRef SprConcept::baseConceptType() const
{
    DynNode* baseConcept = children_[0];

    TypeRef res = baseConcept ? getType(baseConcept) : getConceptType();
    res = adjustMode(res, context_, location_);
    return res;
}

void SprConcept::doSetContextForChildren()
{
    addToSymTab(this);

    if ( !childrenContext_ )
        childrenContext_ = context_->createChildContext(this, effectiveEvalMode(this));

    DynNode::doSetContextForChildren();
}

void SprConcept::doSemanticCheck()
{
    ASSERT(children_.size() == 3);
    DynNode* baseConcept = children_[0];
    DynNode* ifClause = children_[1];
    DynNode*& instantiationsSet = children_[2];
    const string& paramName = getCheckPropertyString("spr.paramName");

    // Compile the base concept node; make sure it's ct
    if ( baseConcept )
    {
        baseConcept->semanticCheck();
        if ( !isCt(baseConcept) )
            REP_ERROR(baseConcept->location(), "Base concept type needs to be compile-time (type=%1%)") % baseConcept->type();
    }

    DynNode* param = baseConcept
        ? mkSprParameter(location_, paramName, baseConcept)
        : mkSprParameter(location_, paramName, getConceptType());
    param->setContext(childrenContext_);
    param->computeType();       // But not semanticCheck, as it will complain of instantiating a var of type auto

    delete instantiationsSet;
    instantiationsSet = new InstantiationsSet(this, { param }, ifClause);
    setExplanation(Feather::mkNop(location_));
}
