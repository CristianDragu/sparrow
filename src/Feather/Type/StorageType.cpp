#include <StdInc.h>
#include "StorageType.h"
#include <Nodes/Properties.h>
#include <Nodes/Decls/Class.h>

#include <Nest/Common/Diagnostic.h>

using namespace Feather;
using namespace Nest;

Class* StorageType::classDecl() const
{
    return data_->referredNode->as<Class>();
}

const string* StorageType::nativeName() const
{
    Class* cls = classDecl();
    return cls ? cls->getPropertyString(propNativeName) : nullptr;
}
