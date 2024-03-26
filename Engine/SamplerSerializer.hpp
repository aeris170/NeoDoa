#pragma once

#include <string>
#include <functional>

#include <tinyxml2.h>

#include <Engine/Sampler.hpp>

/* This is the big bad bada-boom of serializers, this function will call appropriate serializers. See below for an overview! */
std::string SerializeSampler(const Sampler& sampler) noexcept;

/* Here is the general overview */
namespace SamplerSerializer {

    /* ------- Type Definitons ------- */
    using HeaderCommentFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Sampler& sampler)>;
    using SerializeFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Sampler& sampler)>;
    using SerializeNameFunction = std::function<void(tinyxml2::XMLPrinter& printer, const std::string_view name)>;
    using SerializeMinFilterFunction = std::function<void(tinyxml2::XMLPrinter& printer, const TextureMinificationMode mode)>;
    using SerializeMagFilterFunction = std::function<void(tinyxml2::XMLPrinter& printer, const TextureMagnificationMode mode)>;
    using SerializeMinLODFunction = std::function<void(tinyxml2::XMLPrinter& printer, const float minLOD)>;
    using SerializeMaxLODFunction = std::function<void(tinyxml2::XMLPrinter& printer, const float maxLOD)>;
    using SerializeLODBiasFunction = std::function<void(tinyxml2::XMLPrinter& printer, const float LODBias)>;
    using SerializeWrapSFunction = std::function<void(tinyxml2::XMLPrinter& printer, const TextureWrappingMode wrapS)>;
    using SerializeWrapTFunction = std::function<void(tinyxml2::XMLPrinter& printer, const TextureWrappingMode wrapT)>;
    using SerializeWrapRFunction = std::function<void(tinyxml2::XMLPrinter& printer, const TextureWrappingMode wrapR)>;
    using SerializeBorderColorFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Color& borderColor)>;
    using SerializeCompareModeFunction = std::function<void(tinyxml2::XMLPrinter& printer, const TextureCompareMode mode)>;
    using SerializeCompareFunctionFunction = std::function<void(tinyxml2::XMLPrinter& printer, const TextureCompareFunction function)>;
    using SerializeMaxAnisotropyFunction = std::function<void(tinyxml2::XMLPrinter& printer, const float maxAniso)>;
    using SerializeCubemapSeamlessFunction = std::function<void(tinyxml2::XMLPrinter& printer, const bool seamless)>;

    /* --- Default Implementations --- */
    /* These are how NeoDoa will serialize by default. */
    void DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const Sampler& sampler) noexcept;
    void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Sampler& sampler) noexcept;
    void DefaultSerializeName(tinyxml2::XMLPrinter& printer, const std::string_view name) noexcept;
    void DefaultSerializeMinFilter(tinyxml2::XMLPrinter& printer, const TextureMinificationMode mode) noexcept;
    void DefaultSerializeMagFilter(tinyxml2::XMLPrinter& printer, const TextureMagnificationMode mode) noexcept;
    void DefaultSerializeMinLOD(tinyxml2::XMLPrinter& printer, const float minLOD) noexcept;
    void DefaultSerializeMaxLOD(tinyxml2::XMLPrinter& printer, const float maxLOD) noexcept;
    void DefaultSerializeLODBias(tinyxml2::XMLPrinter& printer, const float LODBias) noexcept;
    void DefaultSerializeWrapS(tinyxml2::XMLPrinter& printer, const TextureWrappingMode wrapS) noexcept;
    void DefaultSerializeWrapT(tinyxml2::XMLPrinter& printer, const TextureWrappingMode wrapT) noexcept;
    void DefaultSerializeWrapR(tinyxml2::XMLPrinter& printer, const TextureWrappingMode wrapR) noexcept;
    void DefaultSerializeBorderColor(tinyxml2::XMLPrinter& printer, const Color& borderColor) noexcept;
    void DefaultSerializeCompareMode(tinyxml2::XMLPrinter& printer, const TextureCompareMode mode) noexcept;
    void DefaultSerializeCompareFunction(tinyxml2::XMLPrinter& printer, const TextureCompareFunction function) noexcept;
    void DefaultSerializeMaxAnisotropy(tinyxml2::XMLPrinter& printer, const float maxAniso) noexcept;
    void DefaultSerializeCubemapSeamless(tinyxml2::XMLPrinter& printer, const bool seamless) noexcept;

    /* ----- Serializer Functions ----- */
    inline HeaderCommentFunction HeaderComment{ DefaultHeaderComment };                                  /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeFunction Serialize{ DefaultSerialize };                                              /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeNameFunction SerializeName{ DefaultSerializeName };                                  /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeMinFilterFunction SerializeMinFilter{ DefaultSerializeMinFilter };                   /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeMagFilterFunction SerializeMagFilter{ DefaultSerializeMagFilter };                   /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeMinLODFunction SerializeMinLOD{ DefaultSerializeMinLOD };                            /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeMaxLODFunction SerializeMaxLOD{ DefaultSerializeMaxLOD };                            /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeLODBiasFunction SerializeLODBias{ DefaultSerializeLODBias };                         /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeWrapSFunction SerializeWrapS{ DefaultSerializeWrapS };                               /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeWrapTFunction SerializeWrapT{ DefaultSerializeWrapT };                               /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeWrapRFunction SerializeWrapR{ DefaultSerializeWrapR };                               /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeBorderColorFunction SerializeBorderColor{ DefaultSerializeBorderColor };             /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeCompareModeFunction SerializeCompareMode{ DefaultSerializeCompareMode };             /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeCompareFunctionFunction SerializeCompareFunction{ DefaultSerializeCompareFunction }; /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeMaxAnisotropyFunction SerializeMaxAnisotropy{ DefaultSerializeMaxAnisotropy };       /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeCubemapSeamlessFunction SerializeCubemapSeamless{ DefaultSerializeCubemapSeamless }; /* Feel free to assign this your own function, if you need custom serialization */
}

/* Some Helper functions to trivially serialize (not-so) trivial data types */
namespace SamplerSerializer::Helpers {
    std::string ToString(const TextureMinificationMode mode) noexcept;
    std::string ToString(const TextureMagnificationMode mode) noexcept;
    std::string ToString(const TextureWrappingMode mode) noexcept;
    std::string ToString(const TextureCompareMode mode) noexcept;
    std::string ToString(const TextureCompareFunction function) noexcept;
}