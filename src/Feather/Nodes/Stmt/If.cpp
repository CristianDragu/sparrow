#include <StdInc.h>
#include "If.h"
#include <Feather/FeatherNodeCommonsCpp.h>

#include <Nodes/FeatherNodes.h>
#include <Util/TypeTraits.h>
#include <Util/Ct.h>
#include <Util/Decl.h>


If::If(const Location& location, DynNode* condition, DynNode* thenClause, DynNode* elseClause, bool isCt)
    : DynNode(classNodeKind(), location, {condition, thenClause, elseClause})
{
    if ( isCt )
        setEvalMode(node(), modeCt);
}

DynNode* If::condition() const
{
    return (DynNode*) data_.children[0];
}

DynNode* If::thenClause() const
{
    return (DynNode*) data_.children[1];
}

DynNode* If::elseClause() const
{
    return (DynNode*) data_.children[2];
}

void If::dump(ostream& os) const
{
    os << "if(" << data_.children[0] << ", " << data_.children[1] << ", " << data_.children[2] << ")";
}

void If::doSetContextForChildren()
{
    data_.childrenContext = data_.context->createChildContext(node());

    Nest::setContext(data_.children[0], data_.childrenContext);
    if ( data_.children[1] )
        Nest::setContext(data_.children[1], data_.childrenContext);
    if ( data_.children[2] )
        Nest::setContext(data_.children[2], data_.childrenContext);
}

void If::doSemanticCheck()
{
    // DynNode* condition = (DynNode*) data_.children[0];
    // DynNode* thenClause = (DynNode*) data_.children[1];
    // DynNode* elseClause = (DynNode*) data_.children[2];
    
    // // The resulting type is Void
    // data_.type = getVoidType(data_.context->evalMode());

    // // Semantic check the condition
    // condition->semanticCheck();

    // // Check that the type of the condition is 'Testable'
    // if ( !isTestable(condition->node()) )
    //     REP_ERROR(condition->location(), "The condition of the if is not Testable");

    // // Dereference the condition as much as possible
    // while ( condition->type() && condition->type()->numReferences > 0 )
    // {
    //     condition = mkMemLoad(condition->location(), condition);
    //     condition->setContext(childrenContext());
    //     condition->semanticCheck();
    // }
    // data_.children[0] = condition->node();
    // // TODO (if): Remove this dereference from here

    // if ( nodeEvalMode(node()) == modeCt )
    // {
    //     if ( !isCt(condition->node()) )
    //         REP_ERROR(condition->location(), "The condition of the ct if should be available at compile-time (%1%)") % condition->type();

    //     // Get the CT value from the condition, and select an active branch
    //     Node* c = theCompiler().ctEval(condition->node());
    //     DynNode* selectedBranch = getBoolCtValue(c) ? thenClause : elseClause;

    //     // Expand only the selected branch
    //     if ( selectedBranch )
    //         setExplanation(selectedBranch);
    //     else
    //         setExplanation(mkNop(data_.location));
    //     return;
    // }

    // // Semantic check the clauses
    // if ( thenClause )
    //     thenClause->semanticCheck();
    // if ( elseClause )
    //     elseClause->semanticCheck();
}
