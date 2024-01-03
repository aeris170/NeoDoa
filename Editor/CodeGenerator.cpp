#include <Editor/CodeGenerator.hpp>

#include <format>
#include <sstream>

std::string CodeGenerator::GenerateComponentDeclaration(std::string_view componentName) {
    return std::format(
R"(class {} : Component {{

}};)", componentName);
}

std::string CodeGenerator::GenerateVertexShaderCode() {
    return std::format(
R"(// This is a vertex shader. See https://www.khronos.org/opengl/wiki/Vertex_Shader for further info
// Declares minimum OpenGL version as 4.6, feel free to adjust to your needs.
#version 460 core

// Vertex Attribs
layout(location = 0) in vec3 inPos;  // Input vertex position

// Output Vars

// Uniforms

void main() {{
    gl_Position = vec4(inPos, 1);
}})");
}

std::string CodeGenerator::GenerateTessellationControlShaderCode() {
    return std::format(
R"(// This is a tessellation control shader. See https://www.khronos.org/opengl/wiki/Tessellation_Control_Shader for further info
// Declares minimum OpenGL version as 4.6, feel free to adjust to your needs but take note Tessellation is only supported in OpenGL/GLSL 4.1 or higher.
// This shader assumes you are working with quads, but you can adjust the layout specifier based on the type of patches you are using (e.g., triangles, isolines, etc.).
#version 460 core

// Patch layout
layout (vertices = 4) out;

// Inputs from vertex shader

// Outputs to evaluation shader

void main() {{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // Barrier to ensure all invocations have completed, this can be removed.
    barrier();
}})");
}

std::string CodeGenerator::GenerateTessellationEvaluationShaderCode() {
    return std::format(
R"(// This is a tessellation evaluation shader. See https://www.khronos.org/opengl/wiki/Tessellation_Evaluation_Shader for further info
// Declares minimum OpenGL version as 4.6, feel free to adjust to your needs but take note Tessellation is only supported in OpenGL/GLSL 4.1 or higher.
// This shader assumes you are working with quads, but you can adjust the layout specifier based on the type of patches you are using (e.g., triangles, isolines, etc.).
#version 460 core

// Tessellator layout
layout(quads, equal_spacing, ccw) in;

// Inputs from control shader

// Outputs to fragment shader

void main() {{
    gl_Position = gl_in[gl_InvocationID].gl_Position;

    // Barrier to ensure all invocations have completed, this can be removed.
    barrier();
}})");
}

std::string CodeGenerator::GenerateGeometryShaderCode() {
    return std::format(
R"(// This is a geometry shader. See https://www.khronos.org/opengl/wiki/Geometry_Shader for further info
// Declares minimum OpenGL version as 4.6, feel free to adjust to your needs.
#version 460 core

// Geometry layout
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

// Input Vars

// Outputs

// Uniforms

void main() {{
    for (int i = 0; i < gl_in.length(); ++i) {{
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }}

    EndPrimitive();
}})");
}

std::string CodeGenerator::GenerateFragmentShaderCode() {
    return std::format(
R"(// This is a fragment shader. See https://www.khronos.org/opengl/wiki/Fragment_Shader for further info
// Declares minimum OpenGL version as 4.6, feel free to adjust to your needs.
#version 460 core

// Input Vars

// Outputs
layout(location = 0) out vec4 outColor;

// Uniforms

void main() {{
    outColor = vec4(1, 1, 1, 1);
}})");
}

std::string CodeGenerator::GenerateComputeShaderCode() {
    return std::format(
R"(// This is a compute shader. See https://www.khronos.org/opengl/wiki/Compute_Shader for further info
// Declares minimum OpenGL version as 4.6, feel free to adjust to your needs.
#version 460 core

// Working group layout
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main() {{
    // No-op compute shader
}})");
}

std::string CodeGenerator::GenerateSystemDeclaration(std::string_view systemName) {
    return std::format(
R"(#pragma once

#include "System.hpp"

struct {} {{

    // Init is called once before the first execute
    void Init(entt::registry& reg) {{}}

    // Execute is called every frame
    void Execute(entt::registry& reg, float deltaTime) {{}}
}};)", systemName);
}

std::string CodeGenerator::GenerateBehaviorDeclaration(std::string_view behaviorName) {
    return std::format(
R"(#pragma once

#include "BehaviorComponent.hpp"

struct {} : public BehaviorComponent {{

    // Init is called once before the first execute
    virtual void Init() override {{}}

    // Execute is called every frame
    virtual void Execute(float deltaTime) override {{}}
}};)", behaviorName);
}

std::string CodeGenerator::GenerateRuntimeMain() {
    return "";
}
