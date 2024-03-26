#include <Engine/SamplerDeserializer.hpp>

#include <regex>

#include <Utility/NameOf.hpp>

#include <Engine/Log.hpp>
#include <Engine/FileNode.hpp>

SamplerDeserializationResult DeserializeSampler(const FNode& file) noexcept {
    file.ReadContent();
    auto rv = DeserializeSampler(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedSampler.Name = file.Name();
    }
    return rv;
}

SamplerDeserializationResult DeserializeSampler(const std::string_view data) noexcept {
    SamplerDeserializationResult rv;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.data());
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't deserialize sampler!");
        rv.errors.emplace_back("This should normally never happen. If you didn't try to edit the file manually, please submit an issue.");
        DOA_LOG_ERROR("Couldn't deserialize sampler!\n\n%s", data);
    } else {
        SamplerDeserializer::Deserialize(*doc.RootElement(), rv);
    }

    return rv;
}

void SamplerDeserializer::DefaultDeserialize(tinyxml2::XMLElement& rootElem, SamplerDeserializationResult& sdr) noexcept {
    DeserializeName(rootElem, sdr);
    DeserializeMinFilter(rootElem, sdr);
    DeserializeMagFilter(rootElem, sdr);
    DeserializeMinLOD(rootElem, sdr);
    DeserializeMaxLOD(rootElem, sdr);
    DeserializeLODBias(rootElem, sdr);
    DeserializeWrapS(rootElem, sdr);
    DeserializeWrapT(rootElem, sdr);
    DeserializeWrapR(rootElem, sdr);
    DeserializeBorderColor(rootElem, sdr);
    DeserializeCompareMode(rootElem, sdr);
    DeserializeCompareFunction(rootElem, sdr);
    DeserializeMaxAnisotropy(rootElem, sdr);
    DeserializeCubemapSeamless(rootElem, sdr);
}
void SamplerDeserializer::DefaultDeserializeName(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::Name);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.Name = attribPtr->Value();
}
void SamplerDeserializer::DefaultDeserializeMinFilter(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::MinFilter);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    std::optional<TextureMinificationMode> minMode = Helpers::ToTextureMinificationMode(attribPtr->Value());
    if (!minMode.has_value()) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.MinFilter = minMode.value();
}
void SamplerDeserializer::DefaultDeserializeMagFilter(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::MagFilter);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    std::optional<TextureMagnificationMode> magMode = Helpers::ToTextureMagnificationMode(attribPtr->Value());
    if (!magMode.has_value()) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.MagFilter = magMode.value();
}
void SamplerDeserializer::DefaultDeserializeMinLOD(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::MinLOD);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    float minLOD;
    if (attribPtr->QueryFloatValue(&minLOD) == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.MinLOD = minLOD;
}
void SamplerDeserializer::DefaultDeserializeMaxLOD(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::MaxLOD);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    float maxLOD;
    if (attribPtr->QueryFloatValue(&maxLOD) == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.MaxLOD = maxLOD;
}
void SamplerDeserializer::DefaultDeserializeLODBias(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::LODBias);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    float LODBias;
    if (attribPtr->QueryFloatValue(&LODBias) == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.LODBias = LODBias;
}
void SamplerDeserializer::DefaultDeserializeWrapS(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::WrapS);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    std::optional<TextureWrappingMode> wrapMode = Helpers::ToTextureWrappingMode(attribPtr->Value());
    if (!wrapMode.has_value()) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.WrapS = wrapMode.value();
}
void SamplerDeserializer::DefaultDeserializeWrapT(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::WrapT);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    std::optional<TextureWrappingMode> wrapMode = Helpers::ToTextureWrappingMode(attribPtr->Value());
    if (!wrapMode.has_value()) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.WrapT = wrapMode.value();
}
void SamplerDeserializer::DefaultDeserializeWrapR(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::WrapR);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    std::optional<TextureWrappingMode> wrapMode = Helpers::ToTextureWrappingMode(attribPtr->Value());
    if (!wrapMode.has_value()) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.WrapR = wrapMode.value();
}
void SamplerDeserializer::DefaultDeserializeBorderColor(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::BorderColor);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    static const std::regex regex("Color\\[(\\d*\\.?\\d*)\\, (\\d*\\.?\\d*)\\, (\\d*\\.?\\d*)\\, (\\d*\\.?\\d*)\\]");
    std::smatch match;

    std::string colorString = attribPtr->Value();
    if (!std::regex_match(colorString, match, regex) || match.size() != 5) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data. Regex doesn't match", attribName));
        sdr.erred = true;
        return;
    }
    sdr.deserializedSampler.BorderColor.r = std::stof(match[1]);
    sdr.deserializedSampler.BorderColor.g = std::stof(match[2]);
    sdr.deserializedSampler.BorderColor.b = std::stof(match[3]);
    sdr.deserializedSampler.BorderColor.a = std::stof(match[4]);
}
void SamplerDeserializer::DefaultDeserializeCompareMode(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::CompareMode);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    std::optional<TextureCompareMode> compMode = Helpers::ToTextureCompareMode(attribPtr->Value());
    if (!compMode.has_value()) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.CompareMode = compMode.value();
}
void SamplerDeserializer::DefaultDeserializeCompareFunction(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::CompareFunction);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    std::optional<TextureCompareFunction> compFunc = Helpers::ToTextureCompareFunction(attribPtr->Value());
    if (!compFunc.has_value()) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.CompareFunction = compFunc.value();
}
void SamplerDeserializer::DefaultDeserializeMaxAnisotropy(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::MaxAnisotropy);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    float maxAnisotropy;
    if (attribPtr->QueryFloatValue(&maxAnisotropy) == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.MaxAnisotropy = maxAnisotropy;
}
void SamplerDeserializer::DefaultDeserializeCubemapSeamless(tinyxml2::XMLElement& samplerElem, SamplerDeserializationResult& sdr) noexcept {
    std::string attribName = nameof(Sampler::CubemapSeamless);
    auto* attribPtr = samplerElem.FindAttribute(attribName.c_str());
    if (!attribPtr) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib missing.", attribName));
        sdr.erred = true;
        return;
    }

    bool seamless;
    if (attribPtr->QueryBoolValue(&seamless) == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE) {
        sdr.errors.emplace_back(std::format("Error while deserializing sampler, {} attrib has corrupt data.", attribName));
        sdr.erred = true;
        return;
    }

    sdr.deserializedSampler.CubemapSeamless = seamless;
}

std::optional<TextureMinificationMode> SamplerDeserializer::Helpers::ToTextureMinificationMode(const std::string_view mode) noexcept {
    using enum TextureMinificationMode;
    if (mode == nameof(Nearest))              { return Nearest;              }
    if (mode == nameof(Linear))               { return Linear;               }
    if (mode == nameof(NearestMipmapNearest)) { return NearestMipmapNearest; }
    if (mode == nameof(LinearMipmapNearest))  { return LinearMipmapNearest;  }
    if (mode == nameof(NearestMipmapLinear))  { return NearestMipmapLinear;  }
    if (mode == nameof(LinearMipmapLinear))   { return LinearMipmapLinear;   }
    return std::nullopt;
}
std::optional < TextureMagnificationMode> SamplerDeserializer::Helpers::ToTextureMagnificationMode(const std::string_view mode) noexcept {
    using enum TextureMagnificationMode;
    if (mode == nameof(Nearest)) { return Nearest; }
    if (mode == nameof(Linear))  { return Linear;  }
    return std::nullopt;
}
std::optional<TextureWrappingMode> SamplerDeserializer::Helpers::ToTextureWrappingMode(const std::string_view mode) noexcept {
    using enum TextureWrappingMode;
    if (mode == nameof(Repeat))            { return Repeat;            }
    if (mode == nameof(MirroredRepeat))    { return MirroredRepeat;    }
    if (mode == nameof(ClampToEdge))       { return ClampToEdge;       }
    if (mode == nameof(MirrorClampToEdge)) { return MirrorClampToEdge; }
    if (mode == nameof(ClampToBorder))     { return ClampToBorder;     }
    return std::nullopt;
}
std::optional<TextureCompareMode> SamplerDeserializer::Helpers::ToTextureCompareMode(const std::string_view mode) noexcept {
    using enum TextureCompareMode;
    if (mode == nameof(CompareRefToTexture)) { return CompareRefToTexture; }
    if (mode == nameof(None))                { return None;                }
    return std::nullopt;
}
std::optional<TextureCompareFunction> SamplerDeserializer::Helpers::ToTextureCompareFunction(const std::string_view function) noexcept {
    using enum TextureCompareFunction;
    if(function == nameof(LessEqual))    { return LessEqual;    }
    if(function == nameof(GreaterEqual)) { return GreaterEqual; }
    if(function == nameof(Less))         { return Less;         }
    if(function == nameof(Greater))      { return Greater;      }
    if(function == nameof(Equal))        { return Equal;        }
    if(function == nameof(NotEqual))     { return NotEqual;     }
    if(function == nameof(Always))       { return Always;       }
    if(function == nameof(Never))        { return Never;        }
    return std::nullopt;
}