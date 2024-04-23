#pragma once

#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <string_view>

#include <Engine/Graphics.hpp>

using BufferAllocatorMessage = std::string;

struct GPUBuffer {
    static void BufferSubData(GPUBuffer& buffer, RawDataView dataView, size_t offsetBytes = 0uLL) noexcept;
    static void BufferSubData(GPUBuffer& buffer, size_t sizeBytes, NonOwningPointerToConstRawData data, size_t offsetBytes = 0uLL) noexcept;

    static void GetBufferSubData(const GPUBuffer& buffer, RawDataWriteableView dataView, size_t offsetBytes = 0uLL) noexcept;

    static void CopyBufferSubData(const GPUBuffer& readBuffer, GPUBuffer& writeBuffer, size_t sizeBytesToCopy, size_t readOffsetBytes = 0uLL, size_t writeOffsetBytes = 0uLL) noexcept;

    static void ClearBufferSubData(GPUBuffer& buffer, DataFormat format, size_t sizeBytesToClear, size_t offsetBytes = 0uLL) noexcept;

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
};