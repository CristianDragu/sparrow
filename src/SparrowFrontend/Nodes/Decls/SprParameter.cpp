#include <StdInc.h>
#include "SprParameter.h"
#include <NodeCommonsCpp.h>
#include <Helpers/SprTypeTraits.h>

#include <Feather/Util/Decl.h>

using namespace SprFrontend;
using namespace Nest;

SprParameter::SprParameter(const Location& loc, string name, DynNode* typeNode, DynNode* init)
    : DynNode(classNodeKind(), loc, {typeNode, init})
{
    Feather::setName(node(), move(name));
}

SprParameter::SprParameter(const Location& loc, string name, TypeRef type, DynNode* init)
    : DynNode(classNodeKind(), loc, {nullptr, init})
{
    Feather::setName(node(), move(name));
    setProperty("spr.givenType", type);
}

DynNode* SprParameter::initValue() const
{
    ASSERT(data_.children.size() == 2);
    return (DynNode*) data_.children[1];
}

void SprParameter::dump(ostream& os) const
{
    os << Feather::getName(node()) << ": ";
    if ( data_.type )
        os << data_.type;
    else
    {
        const TypeRef* givenType = getPropertyType("spr.givenType");
        if ( givenType )
            os << *givenType;
        else
            os << data_.children[0];
    }
    if ( data_.children[1] )
        os << " = " << data_.children[1];
}

void SprParameter::doSetContextForChildren()
{
    Feather::addToSymTab(node());

    DynNode::doSetContextForChildren();
}

void SprParameter::doComputeType()
{
    ASSERT(data_.children.size() == 2);
    DynNode* typeNode = (DynNode*) data_.children[0];

    const TypeRef* givenType = getPropertyType("spr.givenType");
    TypeRef t = givenType ? *givenType : getType(typeNode);

    Node* resultingParam = Feather::mkVar(data_.location, Feather::getName(node()), Feather::mkTypeNode(data_.location, t), 0, Feather::effectiveEvalMode(node()));
    Feather::setShouldAddToSymTab(resultingParam, false);
    Nest::setContext(resultingParam, data_.context);
    Nest::computeType(resultingParam);
    data_.explanation = resultingParam;
    data_.type = resultingParam->type;
    Nest::setProperty(node(), Feather::propResultingDecl, resultingParam);
}

void SprParameter::doSemanticCheck()
{
    computeType();

    Nest::semanticCheck(data_.explanation);

    DynNode* init = initValue();
    if ( init )
        init->semanticCheck();
}
