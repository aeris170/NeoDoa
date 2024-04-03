#pragma once

#include <string_view>

enum class ShaderType {
    Vertex,
    TessellationControl,
    TessellationEvaluation,
    Geometry,
    Fragment,
    Compute
};
constexpr std::string_view ToString(ShaderType type) noexcept {
    switch (type) {
        using enum ShaderType;
    case Vertex:                 return "Vertex";
    case TessellationControl:    return "Tessellation Control";
    case TessellationEvaluation: return "Tessellation Evaluation";
    case Geometry:               return "Geometry";
    case Fragment:               return "Fragment";
    case Compute:                return "Compute";
    }
    std::unreachable();
}
