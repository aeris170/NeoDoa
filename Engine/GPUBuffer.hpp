#pragma once

#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <string_view>

#include <Engine/Graphics.hpp>

struct GPUBuffer {
    GLuint GLObjectID{};
#ifdef DEBUG
    std::string Name{};
#endif
    BufferProperties Properties;
    size_t SizeBytes{};

    bool IsDynamicStorage() const noexcept;
    bool IsReadableFromCPU() const noexcept;
    bool IsWriteableFromCPU() const noexcept;
    bool IsPersistent() const noexcept;
    bool IsCoherent() const noexcept;
    bool IsCPUStorage() const noexcept;

    ND_GRAPHICS_COPYABLE_MOVEABLE_RESOURCE(GPUBuffer);
};

struct GPUBufferBuilder {
    GPUBufferBuilder& SetName(std::string_view name) noexcept;
    GPUBufferBuilder& SetProperties(BufferProperties properties) noexcept;
    GPUBufferBuilder& SetStorage(RawDataView dataView) noexcept;
    GPUBufferBuilder& SetStorage(size_t sizeBytes, NonOwningPointerToConstRawData data) noexcept;

    [[nodiscard]] std::pair<std::optional<GPUBuffer>, std::vector<BufferAllocatorMessage>> Build() noexcept;

private:
#ifdef DEBUG
    std::string name{};
#endif
    BufferProperties properties{};
    size_t size{};
    NonOwningPointerToConstRawData data{};

public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUBufferBuilder);

private:
#ifdef OPENGL_4_6_SUPPORT
    friend std::pair<std::optional<GPUBuffer>, std::vector<BufferAllocatorMessage>> Graphics::OpenGL::Build(GPUBufferBuilder&) noexcept;
#endif
};