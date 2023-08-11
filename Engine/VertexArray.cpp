#include <Engine/VertexArray.hpp>

VertexArray::VertexArray() noexcept { AllocateGPU(); }
VertexArray::~VertexArray() noexcept { DeallocateGPU(); }
VertexArray::VertexArray(const VertexArray& other) noexcept : VertexArray() {
    for (size_t i = 0; i < other.buffers.size(); i++) {
        BindVertexAttribBuffer(other.buffers[i], other.layouts[i]);
    }
    BindElementBuffer(other.elemBuffer);
}
VertexArray::VertexArray(VertexArray&& other) noexcept { *this = std::move(other); }
VertexArray& VertexArray::operator=(const VertexArray& other) noexcept {
    // TODO instead of dealloc / alloc, reset everything in VAO?
    DeallocateGPU();
    AllocateGPU();
    buffers.clear();
    layouts.clear();
    for (size_t i = 0; i < other.buffers.size(); i++) {
        BindVertexAttribBuffer(other.buffers[i], other.layouts[i]);
    }
    BindElementBuffer(other.elemBuffer);
    return *this;
}
VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    object = std::exchange(other.object, object);
    buffers = std::move(other.buffers);
    layouts = std::move(other.layouts);
    elemBuffer = std::move(other.elemBuffer);
    return *this;
}

void VertexArray::BindVertexAttribBuffer(const Buffer& buffer, const VertexAttribLayout& layout) noexcept {
    auto bindingIndx { static_cast<GLuint>(buffers.size()) };

    const Buffer& buf{ buffers.emplace_back(buffer) };
    VertexAttribLayout& lay { layouts.emplace_back(layout) };

    glVertexArrayVertexBuffer(object, bindingIndx, buf.object, 0, lay.stride);
    for (size_t i = 0; i < lay.elements.size(); i++) {
        auto&& index{ static_cast<int>(i) };
        const VertexAttribLayout::Element& elem { lay.elements[i] };

        glEnableVertexArrayAttrib(object, index);
        if (elem.type == GL_INT ||
            elem.type == GL_BYTE ||
            elem.type == GL_SHORT ||
            elem.type == GL_UNSIGNED_INT ||
            elem.type == GL_UNSIGNED_BYTE ||
            elem.type == GL_UNSIGNED_SHORT) {
            glVertexArrayAttribIFormat(object, index, elem.count, elem.type, lay.offsets[i]);
        } else if (elem.type == GL_DOUBLE) {
            glVertexArrayAttribLFormat(object, index, elem.count, elem.type, lay.offsets[i]);
        } else {
            glVertexArrayAttribFormat(object, index, elem.count, elem.type, elem.isNormalized, lay.offsets[i]);
        }
        glVertexArrayAttribBinding(object, index, bindingIndx);
    }
}
void VertexArray::BindVertexAttribBuffer(Buffer&& buffer, VertexAttribLayout&& layout) noexcept {
    auto bindingIndx { static_cast<GLuint>(buffers.size()) };

    const Buffer& buf{ buffers.emplace_back(std::move(buffer)) };
    VertexAttribLayout& lay { layouts.emplace_back(std::move(layout)) };

    glVertexArrayVertexBuffer(object, bindingIndx, buf.object, 0, lay.stride);
    for (size_t i = 0; i < lay.elements.size(); i++) {
        auto&& index{ static_cast<int>(i) };
        const VertexAttribLayout::Element& elem { lay.elements[i] };

        glEnableVertexArrayAttrib(object, index);
        if (elem.type == GL_INT ||
            elem.type == GL_BYTE ||
            elem.type == GL_SHORT ||
            elem.type == GL_UNSIGNED_INT ||
            elem.type == GL_UNSIGNED_BYTE ||
            elem.type == GL_UNSIGNED_SHORT) {
            glVertexArrayAttribIFormat(object, index, elem.count, elem.type, lay.offsets[i]);
        } else if (elem.type == GL_DOUBLE) {
            glVertexArrayAttribLFormat(object, index, elem.count, elem.type, lay.offsets[i]);
        } else {
            glVertexArrayAttribFormat(object, index, elem.count, elem.type, elem.isNormalized, lay.offsets[i]);
        }
        glVertexArrayAttribBinding(object, index, bindingIndx);
    }
}
void VertexArray::BindElementBuffer(const Buffer& buffer) noexcept {
    elemBuffer = buffer;
    glVertexArrayElementBuffer(object, elemBuffer.object);
}
void VertexArray::BindElementBuffer(Buffer&& buffer) noexcept {
    elemBuffer = std::move(buffer);
    glVertexArrayElementBuffer(object, elemBuffer.object);
}

void VertexArray::Use() const noexcept { glBindVertexArray(object); }

void VertexArray::AllocateGPU() noexcept { glCreateVertexArrays(1, &object); }
void VertexArray::DeallocateGPU() noexcept { glDeleteVertexArrays(1, &object); }