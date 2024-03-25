#pragma once

#include <string>
#include <optional>
#include <functional>

#include <tinyxml2.h>

#include <Engine/Sampler.hpp>

struct FNode;

struct SamplerDeserializationResult {
    bool erred{ false };
    std::vector<std::string> errors{};
    Sampler deserializedSampler{};
};

/* This is the big bad bada-boom of deserializers, this function will call appropriate deserializers. See below for an overview! */
SamplerDeserializationResult DeserializeSampler(const FNode& file) noexcept;
SamplerDeserializationResult DeserializeSampler(const std::string_view data) noexcept;

/* Here is the general overview */
namespace SamplerDeserializer {

    /* ------- Type Definitons ------- */
    using DeserializeFunction = std::function<void(tinyxml2::XMLElement& rootElem, SamplerDeserializationResult& sdr)>;
    using DeserializeMinFilterFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeMagFilterFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeMinLODFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeMaxLODFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeLODBiasFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeWrapSFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeWrapTFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeWrapRFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeBorderColorFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeCompareModeFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeCompareFunctionFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeMaxAnisotropyFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;
    using DeserializeCubemapSeamlessFunction = std::function<void(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr)>;

    /* --- Default Implementations --- */
    /* These are how NeoDoa will desrialize by default. */
    void DefaultDeserialize(tinyxml2::XMLElement& rootElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeMinFilter(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeMagFilter(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeMinLOD(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeMaxLOD(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeLODBias(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeWrapS(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeWrapT(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeWrapR(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeBorderColor(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeCompareMode(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeCompareFunction(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeMaxAnisotropy(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;
    void DefaultDeserializeCubemapSeamless(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept;

    /* ----- Serializer Functions ----- */
    inline DeserializeFunction Deserialize{ DefaultDeserialize };                                              /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeMinFilterFunction DeserializeMinFilter{ DefaultDeserializeMinFilter };                   /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeMagFilterFunction DeserializeMagFilter{ DefaultDeserializeMagFilter };                   /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeMinLODFunction DeserializeMinLOD{ DefaultDeserializeMinLOD };                            /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeMaxLODFunction DeserializeMaxLOD{ DefaultDeserializeMaxLOD };                            /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeLODBiasFunction DeserializeLODBias{ DefaultDeserializeLODBias };                         /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeWrapSFunction DeserializeWrapS{ DefaultDeserializeWrapS };                               /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeWrapTFunction DeserializeWrapT{ DefaultDeserializeWrapT };                               /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeWrapRFunction DeserializeWrapR{ DefaultDeserializeWrapR };                               /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeBorderColorFunction DeserializeBorderColor{ DefaultDeserializeBorderColor };             /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeCompareModeFunction DeserializeCompareMode{ DefaultDeserializeCompareMode };             /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeCompareFunctionFunction DeserializeCompareFunction{ DefaultDeserializeCompareFunction }; /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeMaxAnisotropyFunction DeserializeMaxAnisotropy{ DefaultDeserializeMaxAnisotropy };       /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeCubemapSeamlessFunction DeserializeCubemapSeamless{ DefaultDeserializeCubemapSeamless }; /* Feel free to assign this your own function, if you need custom deserialization */
}

/* Some Helper functions to trivially serialize (not-so) trivial data types */
namespace SamplerDeserializer::Helpers {
    std::optional<TextureMinificationMode> ToTextureMinificationMode(const std::string_view mode) noexcept;
    std::optional<TextureMagnificationMode> ToTextureMagnificationMode(const std::string_view mode) noexcept;
    std::optional<TextureWrappingMode> ToTextureWrappingMode(const std::string_view mode) noexcept;
    std::optional<TextureCompareMode> ToTextureCompareMode(const std::string_view mode) noexcept;
    std::optional<TextureCompareFunction> ToTextureCompareFunction(const std::string_view function) noexcept;
}