#pragma once

#include "SparrowNodes.h"

namespace SprFrontend {
bool Literal_isString(Node* node);
StringRef Literal_getData(Node* node);

void Class_addChild(Node* cls, Node* child);
} // namespace SprFrontend
