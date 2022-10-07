#pragma once
#include <string>

struct CodeGenerator {

    static std::string GenerateComponentDeclaration(std::string_view componentName);
    static std::string GenerateSystemDeclaration(std::string_view systemName);
    static std::string GenerateBehaviorDeclaration(std::string_view behaviorName);
    static std::string GenerateRuntimeMain();
};