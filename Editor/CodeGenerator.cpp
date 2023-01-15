#include "CodeGenerator.hpp"

#include <format>
#include <sstream>

std::string CodeGenerator::GenerateComponentDeclaration(std::string_view componentName) {
    return std::format("class {} : Component {{\n\n\}};", componentName);
}

std::string CodeGenerator::GenerateSystemDeclaration(std::string_view systemName) {
    std::string s = R"(#pragma once

#include "System.hpp"

struct )"; s.append(systemName); s.append(R"( {

    // Init is called once before the first execute
    void Init(entt::registry& reg) {}

    // Execute is called every frame
    void Execute(entt::registry& reg, float deltaTime) {}
};
)");
    return s;
}

std::string CodeGenerator::GenerateBehaviorDeclaration(std::string_view behaviorName) {
    std::string s = R"(#pragma once

#include "BehaviorComponent.hpp"

struct )"; s.append(behaviorName); s.append(R"( : public BehaviorComponent {

    // Init is called once before the first execute
    virtual void Init() override ;

    // Execute is called every frame
    virtual void Execute(float deltaTime) override ;
};
)");
    return s;
}

std::string CodeGenerator::GenerateRuntimeMain() {
    return "";
}
