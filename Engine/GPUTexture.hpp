#pragma once

#include <array>
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <string_view>

#include <Engine/Graphics.hpp>
#include <Engine/DataTypes.hpp>

// Sampler
struct GPUSampler {
    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{};
#endif

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
#ifdef DEBUG
    std::string name{};
#endif
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

private:
#ifdef OPENGL_4_6_SUPPORT
    friend std::pair<std::optional<GPUSampler>, std::vector<SamplerAllocatorMessage>> Graphics::OpenGL::Build(GPUSamplerBuilder&) noexcept;
#endif
};

// Texture
struct GPUTexture {
    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{};
#endif
    unsigned Width{};
    unsigned Height{};
    unsigned Depth{};
    DataFormat Format{};
    Multisample Samples{};

    bool IsMultisampled() const noexcept;
    operator intptr_t() const;

    ND_GRAPHICS_MOVE_ONLY_RESOURCE(GPUTexture);
};
struct GPUTextureBuilder {
    GPUTextureBuilder& SetName(std::string_view name) noexcept;
    GPUTextureBuilder& SetWidth(unsigned width) noexcept;
    GPUTextureBuilder& SetHeight(unsigned height) noexcept;
    GPUTextureBuilder& SetDepth(unsigned depth) noexcept;
    GPUTextureBuilder& SetData(DataFormat format, RawDataView data) noexcept;
    GPUTextureBuilder& SetSamples(Multisample multisample) noexcept;

    [[nodiscard]] std::pair<std::optional<GPUTexture>, std::vector<TextureAllocatorMessage>> Build() noexcept;

private:
#ifdef DEBUG
    std::string name{};
#endif
    unsigned width{ 1 };
    unsigned height{ 1 };
    unsigned depth{ 1 };
    DataFormat format{};
    RawDataView pixels{};
    Multisample samples{ Multisample::None };
public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUTextureBuilder);

private:
#ifdef OPENGL_4_6_SUPPORT
    friend std::pair<std::optional<GPUTexture>, std::vector<TextureAllocatorMessage>> Graphics::OpenGL::Build(GPUTextureBuilder&) noexcept;
#endif
};
