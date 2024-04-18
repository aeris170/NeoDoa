#include <Engine/GPUTexture.hpp>

#include <cmath>
#include <cassert>
#include <algorithm>

// Sampler
GPUSampler::~GPUSampler() noexcept {
    glDeleteSamplers(1, &GLObjectID);
}
GPUSampler::GPUSampler(GPUSampler&& other) noexcept {
    *this = std::move(other);
}
GPUSampler& GPUSampler::operator=(GPUSampler&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
    Name = std::move(other.Name);
    return *this;
}

GPUSamplerBuilder& GPUSamplerBuilder::SetName(std::string_view name) noexcept {
    this->name = name;
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
    GLuint sampler;
    glGenSamplers(1, &sampler);

    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, ToGLTextureMinificationMode(minFilter));
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, ToGLTextureMagnificationMode(magFilter));
    glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, minLOD);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, maxLOD);
    glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, LODBias);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, ToGLTextureWrappingMode(wrapS));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, ToGLTextureWrappingMode(wrapT));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, ToGLTextureWrappingMode(wrapR));
    glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, borderColor.data());
    glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, ToGLTextureCompareMode(compareMode));
    glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, ToGLTextureCompareFunction(compareFunction));
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    glSamplerParameteri(sampler, GL_TEXTURE_CUBE_MAP_SEAMLESS, cubemapSeamless);

    std::optional<GPUSampler> gpuSampler{ std::nullopt };
    gpuSampler.emplace();
    gpuSampler->GLObjectID = sampler;
    gpuSampler->Name = std::move(name);

    return { std::move(gpuSampler), {} };
}

// Texture
GPUTexture::~GPUTexture() noexcept {
    glDeleteTextures(1, &GLObjectID);
}
GPUTexture::GPUTexture(GPUTexture&& other) noexcept {
    *this = std::move(other);
}
GPUTexture& GPUTexture::operator=(GPUTexture&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
    Name = std::move(other.Name);
    Width = std::exchange(other.Width, {});
    Height = std::exchange(other.Height, {});
    Depth = std::exchange(other.Depth, {});
    Format = std::exchange(other.Format, {});
    return *this;
}

GPUTextureBuilder& GPUTextureBuilder::SetName(std::string_view name) noexcept {
    this->name = name;
    return *this;
}
GPUTextureBuilder& GPUTextureBuilder::SetWidth(unsigned width) noexcept {
    assert(width > 0);
    this->width = width;
    return *this;
}
GPUTextureBuilder& GPUTextureBuilder::SetHeight(unsigned height) noexcept {
    assert(height > 0);
    this->height = height;
    return *this;
}
GPUTextureBuilder& GPUTextureBuilder::SetDepth(unsigned depth) noexcept {
    assert(depth > 0);
    this->depth = depth;
    return *this;
}
GPUTextureBuilder& GPUTextureBuilder::SetData(DataFormat format, RawDataView data) noexcept {
    this->format = format;
    this->pixels = data;
    return *this;
}
std::pair<std::optional<GPUTexture>, std::vector<TextureAllocatorMessage>> GPUTextureBuilder::Build() noexcept {
    GLuint texture;
    if (depth > 1) {
        glCreateTextures(GL_TEXTURE_3D, 1, &texture);
        glTextureStorage3D(
            texture,
            static_cast<GLsizei>(1 + std::floor(std::log2(std::max({ width, height, depth })))),
            ToGLSizedFormat(format),
            width, height, depth
        );
    } else if (width > 1) {
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(
            texture,
            static_cast<GLsizei>(1 + std::floor(std::log2(std::max(width, height)))),
            ToGLSizedFormat(format),
            width, height
        );
    } else {
        glCreateTextures(GL_TEXTURE_1D, 1, &texture);
        glTextureStorage1D(
            texture,
            static_cast<GLsizei>(1 + std::floor(std::log2(width))),
            ToGLSizedFormat(format),
            width
        );
    }

    if (!pixels.empty()) {
        if (depth > 1) {
            glTextureSubImage3D(
                texture,
                0,
                0, 0, 0,
                width, height, depth,
                ToGLBaseFormat(format),
                GL_UNSIGNED_BYTE,
                pixels.data()
            );
        } else if (width > 1) {
            glTextureSubImage2D(
                texture,
                0,
                0, 0,
                width, height,
                ToGLBaseFormat(format),
                GL_UNSIGNED_BYTE,
                pixels.data()
            );
        } else {
            glTextureSubImage1D(
                texture,
                0,
                0,
                width,
                ToGLBaseFormat(format),
                GL_UNSIGNED_BYTE,
                pixels.data()
            );
        }
        glGenerateTextureMipmap(texture);
    }

    std::optional<GPUTexture> gpuTexture{ std::nullopt };
    gpuTexture.emplace();
    gpuTexture->GLObjectID = texture;
    gpuTexture->Name = std::move(name);
    gpuTexture->Width = width;
    gpuTexture->Height = height;
    gpuTexture->Depth = depth;
    gpuTexture->Format = format;

    return { std::move(gpuTexture), {} };
}