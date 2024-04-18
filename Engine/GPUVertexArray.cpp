#include <Engine/GPUVertexArray.hpp>

#include <cassert>

GPUVertexArray::~GPUVertexArray() noexcept {
    glDeleteVertexArrays(1, &GLObjectID);
}
GPUVertexArray::GPUVertexArray(GPUVertexArray&& other) noexcept {
    *this = std::move(other);
}
GPUVertexArray& GPUVertexArray::operator=(GPUVertexArray&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
    ArrayBuffers = std::move(other.ArrayBuffers);
    Layouts = std::move(other.Layouts);
    ElementBuffer = std::move(other.ElementBuffer);
    return *this;
}

bool GPUVertexArray::HasElementBuffer() const noexcept { return ElementBuffer.has_value(); }

GPUVertexArrayBuilder& GPUVertexArrayBuilder::SetName(std::string_view name) noexcept {
    this->name = name;
    return *this;
}
GPUVertexArrayBuilder& GPUVertexArrayBuilder::SetArrayBuffer(const GPUBuffer& buffer, const GPUVertexAttribLayout& layout) noexcept {
    arrayBuffers.emplace_back(buffer);
    layouts.emplace_back(layout);
    return *this;
}
GPUVertexArrayBuilder& GPUVertexArrayBuilder::SetArrayBuffer(GPUBuffer&& buffer, GPUVertexAttribLayout&& layout) noexcept {
    arrayBuffers.emplace_back(std::move(buffer));
    layouts.emplace_back(std::move(layout));
    return *this;
}
GPUVertexArrayBuilder& GPUVertexArrayBuilder::SetElementBuffer(const GPUBuffer& buffer) noexcept {
    elementBuffer.emplace(buffer);
    return *this;
}
GPUVertexArrayBuilder& GPUVertexArrayBuilder::SetElementBuffer(GPUBuffer&& buffer) noexcept {
    elementBuffer.emplace(std::move(buffer));
    return *this;
}

[[nodiscard]] std::pair<std::optional<GPUVertexArray>, std::vector<VertexArrayAllocatorMessage>> GPUVertexArrayBuilder::Build() noexcept {
    assert(arrayBuffers.size() == layouts.size()); // impossible

    GLuint vertexArray;
    glCreateVertexArrays(1, &vertexArray);

    unsigned attribIndex{};
    for (unsigned bindingIndx = 0; bindingIndx < arrayBuffers.size(); bindingIndx++) {
        const GPUBuffer& arrayBuffer{ arrayBuffers[bindingIndx] };
        const GPUVertexAttribLayout& layout{ layouts[bindingIndx] };
        glVertexArrayVertexBuffer(vertexArray, bindingIndx, arrayBuffer.GLObjectID, 0, layout.Stride);
        for (unsigned i = 0; i < layout.Elements.size(); i++) {
            const GPUVertexAttribLayout::Element& elem{ layout.Elements[i] };

            glEnableVertexArrayAttrib(vertexArray, attribIndex);
            if (elem.Type == GL_INT ||
                elem.Type == GL_BYTE ||
                elem.Type == GL_SHORT ||
                elem.Type == GL_UNSIGNED_INT ||
                elem.Type == GL_UNSIGNED_BYTE ||
                elem.Type == GL_UNSIGNED_SHORT) {
                glVertexArrayAttribIFormat(vertexArray, attribIndex, elem.Count, elem.Type, layout.Offsets[i]);
            } else if (elem.Type == GL_DOUBLE) {
                glVertexArrayAttribLFormat(vertexArray, attribIndex, elem.Count, elem.Type, layout.Offsets[i]);
            } else {
                glVertexArrayAttribFormat(vertexArray, attribIndex, elem.Count, elem.Type, elem.IsNormalized, layout.Offsets[i]);
            }
            glVertexArrayAttribBinding(vertexArray, attribIndex, bindingIndx);
            attribIndex++;
        }
    }

    if (elementBuffer) {
        glVertexArrayElementBuffer(vertexArray, elementBuffer->GLObjectID);
    }

    std::optional<GPUVertexArray> gpuVertexArray{ std::nullopt };
    gpuVertexArray.emplace();
    gpuVertexArray->GLObjectID = vertexArray;
    gpuVertexArray->Name = std::move(name);
    gpuVertexArray->ArrayBuffers = std::move(arrayBuffers);
    gpuVertexArray->Layouts = std::move(layouts);
    gpuVertexArray->ElementBuffer = std::move(elementBuffer);

    return { std::move(gpuVertexArray), {} };
}