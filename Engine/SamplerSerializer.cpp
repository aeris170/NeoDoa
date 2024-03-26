#include <Engine/SamplerSerializer.hpp>

#include <Utility/NameOf.hpp>

std::string SerializeSampler(const Sampler& sampler) noexcept {
    tinyxml2::XMLPrinter printer;
    SamplerSerializer::HeaderComment(printer, sampler);
    SamplerSerializer::Serialize(printer, sampler);

    return printer.CStr();
}

void SamplerSerializer::DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const Sampler& sampler) noexcept {
    printer.PushComment("WARNING!! This file is not for editing! Don't!");
}
void SamplerSerializer::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Sampler& sampler) noexcept {
    printer.OpenElement("sampler");
    {
        SerializeName(printer, sampler.Name);
        SerializeMinFilter(printer, sampler.MinFilter);
        SerializeMagFilter(printer, sampler.MagFilter);
        SerializeMinLOD(printer, sampler.MinLOD);
        SerializeMaxLOD(printer, sampler.MaxLOD);
        SerializeLODBias(printer, sampler.LODBias);
        SerializeWrapS(printer, sampler.WrapS);
        SerializeWrapT(printer, sampler.WrapT);
        SerializeWrapR(printer, sampler.WrapR);
        SerializeBorderColor(printer, sampler.BorderColor);
        SerializeCompareMode(printer, sampler.CompareMode);
        SerializeCompareFunction(printer, sampler.CompareFunction);
        SerializeMaxAnisotropy(printer, sampler.MaxAnisotropy);
        SerializeCubemapSeamless(printer, sampler.CubemapSeamless);
    }
    printer.CloseElement();
}
void SamplerSerializer::DefaultSerializeName(tinyxml2::XMLPrinter& printer, const std::string_view name) noexcept {
    printer.PushAttribute(nameof_c(Sampler::Name), name.data());
}
void SamplerSerializer::DefaultSerializeMinFilter(tinyxml2::XMLPrinter& printer, const TextureMinificationMode mode) noexcept {
    printer.PushAttribute(nameof_c(Sampler::MinFilter), Helpers::ToString(mode).c_str());
}
void SamplerSerializer::DefaultSerializeMagFilter(tinyxml2::XMLPrinter& printer, const TextureMagnificationMode mode) noexcept {
    printer.PushAttribute(nameof_c(Sampler::MagFilter), Helpers::ToString(mode).c_str());
}
void SamplerSerializer::DefaultSerializeMinLOD(tinyxml2::XMLPrinter& printer, const float minLOD) noexcept {
    printer.PushAttribute(nameof_c(Sampler::MinLOD), minLOD);
}
void SamplerSerializer::DefaultSerializeMaxLOD(tinyxml2::XMLPrinter& printer, const float maxLOD) noexcept {
    printer.PushAttribute(nameof_c(Sampler::MaxLOD), maxLOD);
}
void SamplerSerializer::DefaultSerializeLODBias(tinyxml2::XMLPrinter& printer, const float LODBias) noexcept {
    printer.PushAttribute(nameof_c(Sampler::LODBias), LODBias);
}
void SamplerSerializer::DefaultSerializeWrapS(tinyxml2::XMLPrinter& printer, const TextureWrappingMode wrapS) noexcept {
    printer.PushAttribute(nameof_c(Sampler::WrapS), Helpers::ToString(wrapS).c_str());
}
void SamplerSerializer::DefaultSerializeWrapT(tinyxml2::XMLPrinter& printer, const TextureWrappingMode wrapT) noexcept {
    printer.PushAttribute(nameof_c(Sampler::WrapT), Helpers::ToString(wrapT).c_str());
}
void SamplerSerializer::DefaultSerializeWrapR(tinyxml2::XMLPrinter& printer, const TextureWrappingMode wrapR) noexcept {
    printer.PushAttribute(nameof_c(Sampler::WrapR), Helpers::ToString(wrapR).c_str());
}
void SamplerSerializer::DefaultSerializeBorderColor(tinyxml2::XMLPrinter& printer, const Color& borderColor) noexcept {
    printer.PushAttribute(nameof_c(Sampler::BorderColor), borderColor.ToString().c_str());
}
void SamplerSerializer::DefaultSerializeCompareMode(tinyxml2::XMLPrinter& printer, const TextureCompareMode mode) noexcept {
    printer.PushAttribute(nameof_c(Sampler::CompareMode), Helpers::ToString(mode).c_str());
}
void SamplerSerializer::DefaultSerializeCompareFunction(tinyxml2::XMLPrinter& printer, const TextureCompareFunction function) noexcept {
    printer.PushAttribute(nameof_c(Sampler::CompareFunction), Helpers::ToString(function).c_str());
}
void SamplerSerializer::DefaultSerializeMaxAnisotropy(tinyxml2::XMLPrinter& printer, const float maxAniso) noexcept {
    printer.PushAttribute(nameof_c(Sampler::MaxAnisotropy), maxAniso);
}
void SamplerSerializer::DefaultSerializeCubemapSeamless(tinyxml2::XMLPrinter& printer, const bool seamless) noexcept {
    printer.PushAttribute(nameof_c(Sampler::CubemapSeamless), seamless);
}

std::string SamplerSerializer::Helpers::ToString(const TextureMinificationMode mode) noexcept {
    switch(mode) {
    using enum TextureMinificationMode;
    case Nearest:              return nameof(Nearest);
    case Linear:               return nameof(Linear);
    case NearestMipmapNearest: return nameof(NearestMipmapNearest);
    case LinearMipmapNearest:  return nameof(LinearMipmapNearest);
    case NearestMipmapLinear:  return nameof(NearestMipmapLinear);
    case LinearMipmapLinear:   return nameof(LinearMipmapLinear);
    }
}
std::string SamplerSerializer::Helpers::ToString(const TextureMagnificationMode mode) noexcept {
    switch(mode) {
    using enum TextureMagnificationMode;
    case Nearest: return nameof(Nearest);
    case Linear:  return nameof(Linear);
    }
}
std::string SamplerSerializer::Helpers::ToString(const TextureWrappingMode mode) noexcept {
    switch(mode) {
    using enum TextureWrappingMode;
    case Repeat:            return nameof(Repeat);
    case MirroredRepeat:    return nameof(MirroredRepeat);
    case ClampToEdge:       return nameof(ClampToEdge);
    case MirrorClampToEdge: return nameof(MirrorClampToEdge);
    case ClampToBorder:     return nameof(ClampToBorder);
    }
}
std::string SamplerSerializer::Helpers::ToString(const TextureCompareMode mode) noexcept {
    switch(mode) {
    using enum TextureCompareMode;
    case CompareRefToTexture: return nameof(CompareRefToTexture);
    case None:                return nameof(None);
    }
}
std::string SamplerSerializer::Helpers::ToString(const TextureCompareFunction function) noexcept {
    switch(function) {
    using enum TextureCompareFunction;
    case LessEqual:    return nameof(LessEqual);
    case GreaterEqual: return nameof(GreaterEqual);
    case Less:         return nameof(Less);
    case Greater:      return nameof(Greater);
    case Equal:        return nameof(Equal);
    case NotEqual:     return nameof(NotEqual);
    case Always:       return nameof(Always);
    case Never:        return nameof(Never);
    }
}