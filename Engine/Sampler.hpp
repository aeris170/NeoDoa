#pragma once

#include <Engine/Color.hpp>

enum class TextureMinificationMode {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
};

enum class TextureMagnificationMode {
    Nearest,
    Linear
};

enum class TextureWrappingMode {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    MirrorClampToEdge,
    ClampToBorder
};


enum class TextureCompareMode {
    CompareRefToTexture,
    None
};

enum class TextureCompareFunction {
    LessEqual,
    GreaterEqual,
    Less,
    Greater,
    Equal,
    NotEqual,
    Always,
    Never
};

struct Sampler {
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
};