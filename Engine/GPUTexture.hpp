#pragma once

#include <array>
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <string_view>

#include <Engine/Graphics.hpp>

// Sampler
using SamplerAllocatorMessage = std::string;

struct GPUSampler {
    GLuint GLObjectID{};
    std::string Name{};

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPUSampler);
};
struct GPUSamplerBuilder {
    GPUSamplerBuilder& SetName(std::string_view name) noexcept;
    GPUSamplerBuilder& SetMinificationFilter(TextureMinificationMode mode) noexcept;
    GPUSamplerBuilder& SetMagnificationFilter(TextureMagnificationMode mode) noexcept;
    GPUSamplerBuilder& SetMinLOD(float lod) noexcept;
    GPUSamplerBuilder& SetMaxLOD(float lod) noexcept;
    GPUSamplerBuilder& SetLODBias(float bias) noexcept;
    GPUSamplerBuilder& SetWrapS(TextureWrappingMode mode) noexcept;
    GPUSamplerBuilder& SetWrapT(TextureWrappingMode mode) noexcept;
    GPUSamplerBuilder& SetWrapR(TextureWrappingMode mode) noexcept;
    GPUSamplerBuilder& SetBorderColor(float r, float g, float b, float a) noexcept;
    GPUSamplerBuilder& SetCompareMode(TextureCompareMode mode) noexcept;
    GPUSamplerBuilder& SetCompareFunction(TextureCompareFunction func) noexcept;
    GPUSamplerBuilder& SetMaxAnisotropy(float value) noexcept;
    GPUSamplerBuilder& SetCubemapSeamless(bool seamless) noexcept;
    [[nodiscard]] std::pair<std::optional<GPUSampler>, std::vector<SamplerAllocatorMessage>> Build() noexcept;

private:
    std::string name{};
    TextureMinificationMode minFilter{ TextureMinificationMode::NearestMipmapLinear };
    TextureMagnificationMode magFilter{ TextureMagnificationMode::Linear };
    float minLOD{ -1000.0f };
    float maxLOD{ 1000.0f };
    float LODBias{ 0.0f };
    TextureWrappingMode wrapS{ TextureWrappingMode::Repeat };
    TextureWrappingMode wrapT{ TextureWrappingMode::Repeat };
    TextureWrappingMode wrapR{ TextureWrappingMode::Repeat };
    std::array<float, 4> borderColor{ 0.0f, 0.0f, 0.0f, 0.0f };
    TextureCompareMode compareMode{ TextureCompareMode::None };
    TextureCompareFunction compareFunction{ TextureCompareFunction::LessEqual };

    float maxAnisotropy{ 1.0f };
    bool cubemapSeamless{ false };

public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUSamplerBuilder);
};

// Texture
using TextureAllocatorMessage = std::string;

struct GPUTexture {
    GLuint GLObjectID{};
    std::string Name{};
    unsigned Width{};
    unsigned Height{};
    unsigned Depth{};
    TextureFormat Format{};

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPUTexture);

    operator void*() const { return reinterpret_cast<void*>(GLObjectID); }
};
struct GPUTextureBuilder {
    GPUTextureBuilder& SetName(std::string_view name) noexcept;
    GPUTextureBuilder& SetWidth(unsigned width) noexcept;
    GPUTextureBuilder& SetHeight(unsigned height) noexcept;
    GPUTextureBuilder& SetDepth(unsigned depth) noexcept;
    GPUTextureBuilder& SetData(TextureFormat format, RawDataView data) noexcept;
    [[nodiscard]] std::pair<std::optional<GPUTexture>, std::vector<TextureAllocatorMessage>> Build() noexcept;

private:
    std::string name{};
    unsigned width{ 1 };
    unsigned height{ 1 };
    unsigned depth{ 1 };
    TextureFormat format{};
    RawDataView pixels{};
public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUTextureBuilder);
};
