#include <Engine/GPUBuffer.hpp>

#include <cassert>

void GPUBuffer::BufferSubData(GPUBuffer& buffer, RawDataView dataView, size_t offsetBytes) noexcept { BufferSubData(buffer, dataView.size_bytes(), dataView.data(), offsetBytes); }
void GPUBuffer::BufferSubData(GPUBuffer& buffer, size_t sizeBytes, NonOwningPointerToConstRawData data, size_t offsetBytes) noexcept {
    assert(static_cast<bool>(buffer.Properties & BufferProperties::DynamicStorage));
    glNamedBufferSubData(buffer.GLObjectID, offsetBytes, sizeBytes, data);
}

void GPUBuffer::GetBufferSubData(const GPUBuffer& buffer, RawDataWriteableView dataView, size_t offsetBytes) noexcept {
    glGetNamedBufferSubData(buffer.GLObjectID, offsetBytes, dataView.size_bytes(), dataView.data());
}

void GPUBuffer::CopyBufferSubData(const GPUBuffer& readBuffer, GPUBuffer& writeBuffer, size_t sizeBytesToCopy, size_t readOffsetBytes, size_t writeOffsetBytes) noexcept {
    assert(readBuffer.GLObjectID > 0);
    assert(readBuffer.GLObjectID > 0);
    glCopyNamedBufferSubData(
        readBuffer.GLObjectID,
        writeBuffer.GLObjectID,
        readOffsetBytes,
        writeOffsetBytes,
        sizeBytesToCopy
    );
}

void GPUBuffer::ClearBufferSubData(GPUBuffer& buffer, DataFormat format, size_t sizeBytesToClear, size_t offsetBytes) noexcept {
    glClearNamedBufferSubData(buffer.GLObjectID, ToGLSizedFormat(format), offsetBytes, sizeBytesToClear, ToGLBaseFormat(format), GL_UNSIGNED_BYTE, nullptr);
}

GPUBuffer::~GPUBuffer() noexcept {
    glDeleteBuffers(1, &GLObjectID);
}
GPUBuffer::GPUBuffer(const GPUBuffer& other) noexcept {
    *this = other;
}
GPUBuffer::GPUBuffer(GPUBuffer&& other) noexcept {
    *this = std::move(other);
}
GPUBuffer& GPUBuffer::operator=(const GPUBuffer& other) noexcept {
    GPUBufferBuilder builder;
    builder
#ifdef DEBUG
        .SetName(other.Name + " (Copy)")
#endif
        .SetProperties(other.Properties);
    auto&& [buffer, _] = builder.Build();

    *this = std::move(buffer.value());
    CopyBufferSubData(other, *this, SizeBytes);
    return *this;
}
GPUBuffer& GPUBuffer::operator=(GPUBuffer&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    Properties = std::exchange(other.Properties, {});
    SizeBytes = std::exchange(other.SizeBytes, {});
    return *this;
}

bool GPUBuffer::IsDynamicStorage() const noexcept   { return static_cast<bool>(Properties & BufferProperties::DynamicStorage);   }
bool GPUBuffer::IsReadableFromCPU() const noexcept  { return static_cast<bool>(Properties & BufferProperties::ReadableFromCPU);  }
bool GPUBuffer::IsWriteableFromCPU() const noexcept { return static_cast<bool>(Properties & BufferProperties::WriteableFromCPU); }
bool GPUBuffer::IsPersistent() const noexcept       { return static_cast<bool>(Properties & BufferProperties::Persistent);       }
bool GPUBuffer::IsCoherent() const noexcept         { return static_cast<bool>(Properties & BufferProperties::Coherent);         }
bool GPUBuffer::IsCPUStorage() const noexcept       { return static_cast<bool>(Properties & BufferProperties::CPUStorage);       }

GPUBufferBuilder& GPUBufferBuilder::SetName(std::string_view name) noexcept {
#ifdef DEBUG
    this->name = name;
#endif
    return *this;
}
GPUBufferBuilder& GPUBufferBuilder::SetProperties(BufferProperties properties) noexcept {
    this->properties = properties;
    return *this;
}
GPUBufferBuilder& GPUBufferBuilder::SetStorage(RawDataView dataView) noexcept {
    size = dataView.size_bytes();
    data = dataView.data();
    return *this;
}
GPUBufferBuilder& GPUBufferBuilder::SetStorage(size_t sizeBytes, NonOwningPointerToConstRawData data) noexcept {
    size = sizeBytes;
    this->data = data;
    return *this;
}

std::pair<std::optional<GPUBuffer>, std::vector<BufferAllocatorMessage>> GPUBufferBuilder::Build() noexcept {
    GLuint buffer;
    glCreateBuffers(1, &buffer);

    glNamedBufferStorage(buffer, size, data, ToGLBufferFlags(properties));

    std::optional<GPUBuffer> gpuBuffer{ std::nullopt };
    gpuBuffer.emplace();
    gpuBuffer->GLObjectID = buffer;
#ifdef DEBUG
    gpuBuffer->Name = std::move(name);
#endif
    gpuBuffer->Properties = properties;
    gpuBuffer->SizeBytes = size;

    return { std::move(gpuBuffer), {} };
}