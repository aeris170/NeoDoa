#include <Engine/FrameBuffer.hpp>

#include <Engine/FrameBufferSerializer.hpp>
#include <Engine/FrameBufferDeserializer.hpp>

std::string FrameBuffer::Serialize() const noexcept {
    return SerializeFrameBuffer(*this);
}
FrameBuffer FrameBuffer::Deserialize(const std::string_view data) noexcept {
    return DeserializeFrameBuffer(data).deserializedFrameBuffer;
}

FrameBuffer FrameBuffer::Copy(const FrameBuffer& frameBuffer) noexcept {
    return FrameBuffer::Deserialize(frameBuffer.Serialize());
}