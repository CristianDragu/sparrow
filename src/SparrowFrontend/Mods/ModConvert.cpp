#include <StdInc.h>
#include "ModConvert.h"

#include <Nodes/SprProperties.h>

#include <Feather/Util/Decl.h>
#include <Nest/Common/Diagnostic.h>

using namespace SprFrontend;

void ModConvert::beforeComputeType(DynNode* node)
{
    /// Check to apply only to constructors
    if ( node->nodeKind() != nkSparrowDeclSprFunction || Feather::getName(node) != "ctor" )
        REP_INTERNAL(node->location(), "convert modifier can be applied only to constructors");

    node->setProperty(propConvert, 1);
}
