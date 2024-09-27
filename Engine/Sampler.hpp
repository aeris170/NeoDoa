#pragma once

#include <Engine/Color.hpp>
#include <Engine/Graphics.hpp>

struct Sampler {
    std::string Name{};
    TextureMinificationMode MinFilter{ TextureMinificationMode::NearestMipmapLinear };
    TextureMagnificationMode MagFilter{ TextureMagnificationMode::Linear };
    float MinLOD{ -1000.0f };
    float MaxLOD{ 1000.0f };
    float LODBias{ 0.0f };
    TextureWrappingMode WrapS{ TextureWrappingMode::Repeat };
    TextureWrappingMode WrapT{ TextureWrappingMode::Repeat };
    TextureWrappingMode WrapR{ TextureWrappingMode::Repeat };
    Color BorderColor{ 0.0f, 0.0f, 0.0f, 0.0f };
    TextureCompareMode CompareMode{ TextureCompareMode::None };
    TextureCompareFunction CompareFunction{ TextureCompareFunction::LessEqual };

    float MaxAnisotropy{ 1.0f };
    bool CubemapSeamless{ false };

    std::string Serialize() const noexcept;
    static Sampler Deserialize(const std::string_view data) noexcept;

    static Sampler Copy(const Sampler& sampler) noexcept;
};