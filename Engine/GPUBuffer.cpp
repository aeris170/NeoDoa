#include <Engine/GPUBuffer.hpp>

#include <cassert>

GPUBuffer::~GPUBuffer() noexcept {
    Graphics::Destructors::Destruct(*this);
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
    Graphics::CopyBufferSubData(other, *this, SizeBytes);
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
    return Graphics::Builders::Build(*this);
}