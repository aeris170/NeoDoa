#include <Engine/Buffer.hpp>

Buffer::Buffer(const OpenGL::BufferData& data) noexcept :
    data(data) {
    AllocateGPU();
}
Buffer::Buffer(OpenGL::BufferData&& data) noexcept :
    data(std::move(data)) {
    AllocateGPU();
}
Buffer::~Buffer() noexcept { DeallocateGPU(); }
Buffer::Buffer(const Buffer& other) noexcept :
    data(other.data) {
    AllocateGPU();
}
Buffer::Buffer(Buffer&& other) noexcept :
    object(std::exchange(other.object, 0)),
    data(std::move(other.data)) {}
Buffer& Buffer::operator=(const Buffer& other) noexcept {
    DeallocateGPU();
    data = other.data;
    AllocateGPU();
    return *this;
}
Buffer& Buffer::operator=(Buffer&& other) noexcept {
    DeallocateGPU();
    object = std::exchange(other.object, 0);
    data = std::move(other.data);
    return *this;
}

bool Buffer::HasData() const { return object != 0; }
const OpenGL::BufferData& Buffer::GetData() const { return data; }
OpenGL::BufferData::const_iterator Buffer::begin() const { return data.begin(); }
OpenGL::BufferData::const_iterator Buffer::end() const { return data.end(); }

void Buffer::AllocateGPU() noexcept {
    glCreateBuffers(1, &object);
    glNamedBufferStorage(object, sizeof(OpenGL::BufferData::value_type) * data.size(), data.data(), 0);
}
void Buffer::DeallocateGPU() noexcept {
    glDeleteBuffers(1, &object);
}

