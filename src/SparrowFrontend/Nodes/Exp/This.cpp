#include <StdInc.h>
#include "This.h"
#include <NodeCommonsCpp.h>

using namespace SprFrontend;

This::This(const Location& loc)
    : Node(loc)
{
}

void This::doSemanticCheck()
{
    setExplanation(mkIdentifier(location_, "$this"));
}
