#include <StdInc.h>
#include "ModInitCtor.h"

#include <Nodes/SprProperties.h>

#include <Nest/Common/Diagnostic.h>

using namespace SprFrontend;

void ModInitCtor::beforeComputeType(DynNode* node)
{
    /// Check to apply only to classes
    if ( node->nodeKind() != nkSparrowDeclSprClass )
        REP_ERROR(node->location(), "initCtor modifier can be applied only to classes");
    
    node->setProperty(propGenerateInitCtor, 1);
}
