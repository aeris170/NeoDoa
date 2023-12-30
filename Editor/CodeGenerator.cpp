#include "CodeGenerator.hpp"

#include <format>
#include <sstream>

std::string CodeGenerator::GenerateComponentDeclaration(std::string_view componentName) {
    return std::format("class {} : Component {{\n\n}};", componentName);
}

std::string CodeGenerator::GenerateVertexShaderCode() {
    return R"(// This is a vertex shader. See https://www.khronos.org/opengl/wiki/Vertex_Shader for further info
// Declares minimum OpenGL version as 4.6, feel free to adjust to your needs.
#version 460 core

// Vertex Attribs

// Output Vars

// Uniforms

void main() {
    gl_Position = vec4(0, 0, 0, 1);
}
)";
}

std::string CodeGenerator::GenerateTessellationControlShaderCode() {
    return {};
}

std::string CodeGenerator::GenerateTessellationEvaluationShaderCode() {
    return {};
}

std::string CodeGenerator::GenerateGeometryShaderCode() {
    return {};
}

std::string CodeGenerator::GenerateFragmentShaderCode() {
    return {};
}

std::string CodeGenerator::GenerateComputeShaderCode() {
    return {};
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
    virtual void Init() override {}

    // Execute is called every frame
    virtual void Execute(float deltaTime) override {}
};
)");
    return s;
}

std::string CodeGenerator::GenerateRuntimeMain() {
    return "";
}
