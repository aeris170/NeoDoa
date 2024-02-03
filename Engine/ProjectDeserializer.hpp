#pragma once

#include <Engine/Project.hpp>

struct FNode;

struct ProjectDeserializationResult {
    bool erred{ false };
    Project project{};
};

ProjectDeserializationResult DeserializeProject(const FNode* file);