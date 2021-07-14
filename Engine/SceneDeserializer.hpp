#pragma once

#include "Scene.hpp"

struct FNode;

std::shared_ptr<Scene> DeserializeScene(FNode* file);