#include "Mesh.hpp"

#include <cstddef>

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices) noexcept :
    _vertices(std::move(vertices)),
    _indices(std::move(indices)) {
    UpdateVRAM();
}

Mesh::~Mesh() { DeAlloc(); }

Mesh::Mesh(Mesh&& other) noexcept :
    _vertices(std::move(other._vertices)),
    _indices(std::move(other._indices)),
    _vao(other._vao),
    _vbo(other._vbo),
    _ebo(other._ebo) {
    UpdateVRAM();
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    DeAlloc();
    _vertices = std::move(other._vertices);
    _indices = std::move(other._indices);
    _vao = other._vao;
    other._vao = 0;
    _vbo = other._vbo;
    other._vbo = 0;
    _ebo = other._ebo;
    other._ebo = 0;
    UpdateVRAM();
    return *this;
}

VAO Mesh::VertexArrayObject() const { return _vao; }
bool Mesh::HasIndexBuffer() const { return _indices.size() > 0; }
EBO Mesh::ElementBufferObject() const { return _ebo; }

std::vector<Vertex>& Mesh::Vertices() { return _vertices; }
const std::vector<Vertex>& Mesh::Vertices() const { return _vertices; }

std::vector<GLuint>& Mesh::Indices() { return _indices; }
const std::vector<GLuint>& Mesh::Indices() const { return _indices; }

void Mesh::DeAlloc() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    _vao = 0;
    _vbo = 0;
    _ebo = 0;

    _vertices.clear();
    _indices.clear();
}

void Mesh::UpdateVRAM() {
    glCreateVertexArrays(1, &_vao);

    auto vertexCount = _vertices.size();
    auto vertexSize = sizeof(Vertex);

    auto totalSize = vertexCount * vertexSize;

    glCreateBuffers(1, &_vbo);
    //glNamedBufferData(_vbo, totalSize, _vertices.data(), GL_STATIC_DRAW); // if crash, driver bug
    glNamedBufferStorage(_vbo, totalSize, _vertices.data(), 0); // TODO investigate this shit

    glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, static_cast<GLsizei>(vertexSize));

    glEnableVertexArrayAttrib(_vao, 0);
    glEnableVertexArrayAttrib(_vao, 1);
    glEnableVertexArrayAttrib(_vao, 2);
    glEnableVertexArrayAttrib(_vao, 3);

    glVertexArrayAttribBinding(_vao, 0, 0);
    glVertexArrayAttribBinding(_vao, 1, 0);
    glVertexArrayAttribBinding(_vao, 2, 0);
    glVertexArrayAttribBinding(_vao, 3, 0);

    glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribFormat(_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribFormat(_vao, 2, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
    glVertexArrayAttribFormat(_vao, 3, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

    if (HasIndexBuffer()) {
        glCreateBuffers(1, &_ebo);
        glNamedBufferData(_ebo, _indices.size() * sizeof(_indices[0]), _indices.data(), GL_STATIC_DRAW);
        glVertexArrayElementBuffer(_vao, _ebo);
    }
}