#include <Engine/GPUTexture.hpp>

#include <cmath>
#include <cassert>
#include <algorithm>

// Sampler
GPUSampler::~GPUSampler() noexcept {
    Graphics::Destructors::Destruct(*this);
}
GPUSampler::GPUSampler(GPUSampler&& other) noexcept {
    *this = std::move(other);
}
GPUSampler& GPUSampler::operator=(GPUSampler&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    return *this;
}

GPUSamplerBuilder& GPUSamplerBuilder::SetName(std::string_view name) noexcept {
#ifdef DEBUG
    this->name = name;
#endif
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetMinificationFilter(TextureMinificationMode mode) noexcept {
    minFilter = mode;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetMagnificationFilter(TextureMagnificationMode mode) noexcept {
    magFilter = mode;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetMinLOD(float lod) noexcept {
    minLOD = lod;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetMaxLOD(float lod) noexcept {
    maxLOD = lod;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetLODBias(float bias) noexcept {
    LODBias = bias;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetWrapS(TextureWrappingMode mode) noexcept {
    wrapS = mode;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetWrapT(TextureWrappingMode mode) noexcept {
    wrapT = mode;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetWrapR(TextureWrappingMode mode) noexcept {
    wrapR = mode;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetBorderColor(float r, float b, float g, float a) noexcept {
    borderColor[0] = std::clamp(r, 0.0f, 1.0f);
    borderColor[1] = std::clamp(g, 0.0f, 1.0f);
    borderColor[2] = std::clamp(b, 0.0f, 1.0f);
    borderColor[3] = std::clamp(a, 0.0f, 1.0f);
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetCompareMode(TextureCompareMode mode) noexcept {
    compareMode = mode;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetCompareFunction(TextureCompareFunction func) noexcept {
    compareFunction = func;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetMaxAnisotropy(float value) noexcept {
    maxAnisotropy = value;
    return *this;
}
GPUSamplerBuilder& GPUSamplerBuilder::SetCubemapSeamless(bool seamless) noexcept {
    cubemapSeamless = seamless;
    return *this;
}
std::pair<std::optional<GPUSampler>, std::vector<SamplerAllocatorMessage>> GPUSamplerBuilder::Build() noexcept {
    return Graphics::Builders::Build(*this);
}

// Texture
GPUTexture::~GPUTexture() noexcept {
    Graphics::Destructors::Destruct(*this);
}
GPUTexture::GPUTexture(GPUTexture&& other) noexcept {
    *this = std::move(other);
}
GPUTexture& GPUTexture::operator=(GPUTexture&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    Width = std::exchange(other.Width, {});
    Height = std::exchange(other.Height, {});
    Depth = std::exchange(other.Depth, {});
    Format = std::exchange(other.Format, {});
    Samples = std::exchange(other.Samples, {});
    return *this;
}

bool GPUTexture::IsMultisampled() const noexcept { return Samples != Multisample::None; }
GPUTexture::operator void* () const { return reinterpret_cast<void*>(static_cast<uint64_t>(GLObjectID)); }

GPUTextureBuilder& GPUTextureBuilder::SetName(std::string_view name) noexcept {
#ifdef DEBUG
    this->name = name;
#endif
    return *this;
}
GPUTextureBuilder& GPUTextureBuilder::SetWidth(unsigned width) noexcept {
    this->width = width;
    return *this;
}
GPUTextureBuilder& GPUTextureBuilder::SetHeight(unsigned height) noexcept {
    this->height = height;
    return *this;
}
GPUTextureBuilder& GPUTextureBuilder::SetDepth(unsigned depth) noexcept {
    this->depth = depth;
    return *this;
}
GPUTextureBuilder& GPUTextureBuilder::SetSamples(Multisample multisample) noexcept {
    samples = multisample;
    return *this;
}
GPUTextureBuilder& GPUTextureBuilder::SetData(DataFormat format, RawDataView data) noexcept {
    this->format = format;
    this->pixels = data;
    return *this;
}

std::pair<std::optional<GPUTexture>, std::vector<TextureAllocatorMessage>> GPUTextureBuilder::Build() noexcept {
    return Graphics::Builders::Build(*this);
}