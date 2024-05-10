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
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    ArrayBuffers = std::move(other.ArrayBuffers);
    Layouts = std::move(other.Layouts);
    ElementBuffer = std::move(other.ElementBuffer);
    std::swap(IndexType, other.IndexType);
    std::swap(Topology, other.Topology);
    return *this;
}

bool GPUVertexArray::HasElementBuffer() const noexcept { return ElementBuffer.has_value(); }

GPUVertexArrayBuilder& GPUVertexArrayBuilder::SetName(std::string_view name) noexcept {
#ifdef DEBUG
    this->name = name;
#endif
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
GPUVertexArrayBuilder& GPUVertexArrayBuilder::SetElementBuffer(const GPUBuffer& buffer, DataType indexType) noexcept {
    elementBuffer.emplace(buffer);
    this->indexType = indexType;
    return *this;
}
GPUVertexArrayBuilder& GPUVertexArrayBuilder::SetElementBuffer(GPUBuffer&& buffer, DataType indexType) noexcept {
    elementBuffer.emplace(std::move(buffer));
    this->indexType = indexType;
    return *this;
}
GPUVertexArrayBuilder& GPUVertexArrayBuilder::SetTopology(TopologyType topology) noexcept {
    this->topology = topology;
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
        glVertexArrayBindingDivisor(vertexArray, bindingIndx, layout.Divisor);
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
#ifdef DEBUG
    gpuVertexArray->Name = std::move(name);
#endif
    gpuVertexArray->ArrayBuffers = std::move(arrayBuffers);
    gpuVertexArray->Layouts = std::move(layouts);
    gpuVertexArray->ElementBuffer = std::move(elementBuffer);
    gpuVertexArray->IndexType = indexType;
    gpuVertexArray->Topology = topology;

    return { std::move(gpuVertexArray), {} };
}