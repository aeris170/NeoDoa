#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <utility>
#include <string_view>

#include <Engine/UUID.hpp>
#include <Engine/Color.hpp>

#include <Utility/Tree.hpp>

struct Model {
    struct Node {
        std::string Name{};
        std::vector<size_t> MeshIndices{};
    };
    struct Mesh {
        UUID MeshUUID;
        size_t MaterialIndex;
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
            None,
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
        std::optional<ShadingMode> ShadingMode{};
        std::optional<BlendMode> BlendMode{};
        std::optional<float> Opacity{};
        std::optional<float> Shininess{};
        std::optional<float> ShininessStrength{};
        std::optional<float> Refracti{};
        std::array<TextureStack, TextureTypeCount> TextureStacks{};
    };

    std::string Name{};
    Tree<Node> Nodes{};
    std::vector<Mesh> Meshes{};
    std::vector<Texture> Textures{};
    std::vector<Material> Materials{};

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