#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <utility>
#include <optional>
#include <string_view>

#include <Engine/Mesh.hpp>
#include <Engine/UUID.hpp>
#include <Engine/Color.hpp>

#include <Utility/Tree.hpp>

struct Model {
    struct Node {
        std::string Name{};
        std::vector<size_t> MeshIndices{};
    };
    struct Mesh {
        unsigned Count;
        int BaseVertex;
        unsigned BaseIndex;
        unsigned MaterialIndex;
    };
    struct Texture {
        UUID TextureUUID;
        bool Embedded;
    };
    struct Material {
        enum class TextureOperation : uint8_t {
            Multiply,  /* T = T1 * T2 */
            Add,       /* T = T1 + T2 */
            Subtract,  /* T = T1 - T2 */
            Divide,    /* T = T1 / T2 */
            SmoothAdd, /* T = (T1 + T2) - (T1 * T2) */
            SignedAdd  /* T = T1 + (T2-0.5) */
        };
        enum class TextureMapMode : uint8_t {
            Wrap,  /* Texture coordinate u|v is translated to u%1|v%1. Analogous to TextureWrappingMode::Repeat. */
            Clamp, /* Texture coordinates outside [0...1] are clamped to the nearest valid value. Analogous to TextureWrappingMode::ClampToEdge. */
            Decal, /* Texture coordinates outside [0...1] makes texture not applied to that pixel. Have no analog, handle in shader. */
            Mirror /* Like wrap but mirrors. Analogous to TextureWrappingMode::MirroredRepeat. */
        };
        enum class TextureMapping : uint8_t {
            UV,       /* Standard UV mapping. */
            Sphere,   /* Spherical mapping */
            Cylinder, /* Cylindrical mapping */
            Box,      /* Cubic mapping */
            Plane,    /* Planar mapping */
            Undefined /* Undefined mapping. Have fun. */
        };
        enum class TextureType : uint8_t {
            Diffuse,
            Specular,
            Ambient,
            Emissive,
            Height,
            Normals,
            Shininess,
            Opacity,
            Displacement,
            LightMap,
            Reflection,
            BaseColor,
            NormalCamera,
            EmissionColor,
            Metalness,
            DiffuseRougness,
            AmbientOcclusion,
            Unknown,
            Sheen,
            Clearcoat,
            Transmission,
            MayaBase,
            MayaSpecular,
            MayaSpecularColor,
            MayaSpecularRoughness
        };
        enum class ShadingMode : uint8_t {
            Flat,
            Gouraud,
            Phong,
            Blinn,
            Toon,
            OrenNayar,
            Minnaert,
            CookTorrance,
            Unlit,
            Fresnel,
            PBRBRDF,
        };
        enum class TextureFlags : uint8_t {
            Invert = (1 << 0),      /* The texture's color values have to be inverted (component-wise 1-n) */
            UseAlpha = (1 << 1),    /* Explicit request to the application to process the alpha channel of the texture. Mutually exclusive with TextureFlags::IgnoreAlpha. */
            IgnoreAlpha = (1 << 2), /* Explicit request to the application to ignore the alpha channel of the texture. Mutually exclusive with TextureFlags::UseAlpha. */
        };
        enum class BlendMode : uint8_t {
            Default,  /* SourceColor * SourceAlpha + DestColor * (1 - SourceAlpha) */
            Additive, /* SourceColor + DestColor */
        };
        struct TextureInfo {
            std::string Path;
            float BlendAmount;
            TextureOperation Operation;
            TextureMapping Mapping;
            int UVWSource;
            TextureMapMode MapModeU, MapModeV;
            glm::vec3 MappingAxis;
            TextureFlags Flags;
        };
        struct TextureStack {
            size_t TextureCount;
            std::array<TextureInfo, 4> Textures;
        };
        constexpr static size_t TextureTypeCount = std::to_underlying(TextureType::MayaSpecularRoughness) + 1;

        std::optional<std::string> Name{};
        std::optional<Color> DiffuseColor{};
        std::optional<Color> SpecularColor{};
        std::optional<Color> AmbientColor{};
        std::optional<Color> EmissiveColor{};
        std::optional<Color> TransparentColor{};
        std::optional<Color> ReflectiveColor{};
        std::optional<float> Reflectivity{};
        std::optional<bool> EnableWireframe{};
        std::optional<bool> TwoSided{};
        std::optional<ShadingMode> Shading{};
        std::optional<BlendMode> Blend{};
        std::optional<float> Opacity{};
        std::optional<float> Shininess{};
        std::optional<float> ShininessStrength{};
        std::optional<float> Refracti{};
        std::array<TextureStack, TextureTypeCount> TextureStacks{};
    };

    std::string Name{};
    Tree<Node> Nodes{};
    std::vector<std::string> MeshNames{};
    std::vector<UUID> MeshUUIDs{};
    std::vector<Mesh> Meshes{};
    std::vector<Texture> Textures{};
    std::vector<Material> Materials{};
    ::Mesh UnifiedModelMesh{};

    std::string Serialize() const noexcept;
    static Model Deserialize(const std::string_view data) noexcept;

    static Model Copy(const Model& model) noexcept;
};

constexpr Model::Material::TextureFlags operator &(const Model::Material::TextureFlags lhs, const Model::Material::TextureFlags rhs) {
    return static_cast<Model::Material::TextureFlags>(std::to_underlying(lhs) & std::to_underlying(rhs));
}
constexpr Model::Material::TextureFlags operator |(const Model::Material::TextureFlags lhs, const Model::Material::TextureFlags rhs) {
    return static_cast<Model::Material::TextureFlags>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

constexpr std::string_view ToString(Model::Material::TextureOperation operation) noexcept {
    using enum Model::Material::TextureOperation;
    switch (operation) {
    case Multiply:  return "Multiply (T = T1 * T2)";
    case Add:       return "Add (T = T1 + T2)";
    case Subtract:  return "Subtract (T = T1 - T2)";
    case Divide:    return "Divide (T = T1 / T2)";
    case SmoothAdd: return "SmoothAdd (T = (T1 + T2) - (T1 * T2))";
    case SignedAdd: return "SignedAdd (T = T1 + (T2 - 0.5))";
    }
    std::unreachable();
}
constexpr std::string_view ToString(Model::Material::TextureMapMode mapMode) noexcept {
    using enum Model::Material::TextureMapMode;
    switch (mapMode) {
    case Wrap:   return "Wrap";
    case Clamp:  return "Clamp";
    case Decal:  return "Decal";
    case Mirror: return "Mirror";
    }
    std::unreachable();
}
constexpr std::string_view ToString(Model::Material::TextureMapping mapping) noexcept {
    using enum Model::Material::TextureMapping;
    switch (mapping) {
    case UV:        return "UV";
    case Sphere:    return "Sphere";
    case Cylinder:  return "Cylinder";
    case Box:       return "Box";
    case Plane:     return "Plane";
    case Undefined: return "Undefined";
    }
    std::unreachable();
}
constexpr std::string_view ToString(Model::Material::TextureType type) noexcept {
    using enum Model::Material::TextureType;
    switch (type) {
        case Diffuse:               return "Diffuse";
        case Specular:              return "Specular Map";
        case Ambient:               return "Ambient";
        case Emissive:              return "Emissive Map";
        case Height:                return "Height Map";
        case Normals:               return "Normals";
        case Shininess:             return "Shininess";
        case Opacity:               return "Opacity Map";
        case Displacement:          return "Displacement Map";
        case LightMap:              return "Light Map";
        case Reflection:            return "Reflection";
        case BaseColor:             return "Albedo (Base Color)";
        case NormalCamera:          return "Normal Map";
        case EmissionColor:         return "Emission Map";
        case Metalness:             return "Metallic Map";
        case DiffuseRougness:       return "Rougness Map";
        case AmbientOcclusion:      return "Ambient Occlusion Map";
        case Unknown:               return "Unknown";
        case Sheen:                 return "Sheen";
        case Clearcoat:             return "Clearcoat";
        case Transmission:          return "Transmission";
        case MayaBase:              return "Base (Maya)";
        case MayaSpecular:          return "Specular (Maya)";
        case MayaSpecularColor:     return "Specular Color (Maya)";
        case MayaSpecularRoughness: return "Specular Roughness (Maya)";
    }
    std::unreachable();
}
constexpr std::string_view ToString(Model::Material::ShadingMode shadingMode) noexcept {
    using enum Model::Material::ShadingMode;
    switch (shadingMode) {
    case Flat:         return "Flat";
    case Gouraud:      return "Gouraud";
    case Phong:        return "Phong";
    case Blinn:        return "Blinn";
    case Toon:         return "Toon";
    case OrenNayar:    return "Oren-Nayar";
    case Minnaert:     return "Minnaert";
    case CookTorrance: return "Cook-Torrance";
    case Unlit:        return "Unlit";
    case Fresnel:      return "Fresnel";
    case PBRBRDF:      return "PBR-BRDF";
    }
    std::unreachable();
}
constexpr std::string_view ToString(Model::Material::BlendMode blendMode) noexcept {
    using enum Model::Material::BlendMode;
    switch (blendMode) {
    case Default:  return "Default";
    case Additive: return "Additive";
    }
    std::unreachable();
}
constexpr std::string ToString(Model::Material::TextureFlags flags) noexcept {
    if (static_cast<uint8_t>(flags) == 0) { return "None"; }

    std::string result;
    using enum Model::Material::TextureFlags;
    if (static_cast<bool>(flags & Invert))      result = "Invert";
    if (static_cast<bool>(flags & UseAlpha))    result += (result.empty() ? "" : " | ") + std::string("Use Alpha");
    if (static_cast<bool>(flags & IgnoreAlpha)) result += (result.empty() ? "" : " | ") + std::string("Ignore Alpha");

    return result;
}