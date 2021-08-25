#include "Mesh.hpp"

#include <cstddef>
#include "Log.hpp"

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, std::vector<std::weak_ptr<Texture>>&& textures) noexcept :
	_vertices(std::move(vertices)),
	_indices(std::move(indices)),
	_textures(std::move(textures)) {
	glCreateVertexArrays(1, &_vao);

	auto vertexCount = _vertices.size();
	auto vertexSize = sizeof(Vertex);

	auto totalSize = vertexCount * vertexSize;

	glCreateBuffers(1, &_vbo);
	//glNamedBufferData(_vbo, totalSize, _vertices.data(), GL_STATIC_DRAW); // if crash, driver bug
	glNamedBufferStorage(_vbo, totalSize, _vertices.data(), 0); // TODO investigate this shit

	glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, vertexSize);

	glEnableVertexArrayAttrib(_vao, 4);
	glEnableVertexArrayAttrib(_vao, 5);
	glEnableVertexArrayAttrib(_vao, 6);
	glEnableVertexArrayAttrib(_vao, 7);
	glEnableVertexArrayAttrib(_vao, 8);

	glVertexArrayAttribBinding(_vao, 4, 0);
	glVertexArrayAttribBinding(_vao, 5, 0);
	glVertexArrayAttribBinding(_vao, 6, 0);
	glVertexArrayAttribBinding(_vao, 7, 0);
	glVertexArrayAttribBinding(_vao, 8, 0);

	glVertexArrayAttribFormat(_vao, 4, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribFormat(_vao, 5, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribFormat(_vao, 6, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
	glVertexArrayAttribFormat(_vao, 7, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	glVertexArrayAttribIFormat(_vao, 8, 1, GL_SHORT, offsetof(Vertex, texIndex));

	if (_indices.size() > 0) {
		glCreateBuffers(1, &_ebo);
		glNamedBufferData(_ebo, _indices.size() * sizeof(_indices[0]), _indices.data(), GL_STATIC_DRAW);
		glVertexArrayElementBuffer(_vao, _ebo);
	}
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}

Mesh::Mesh(Mesh&& other) noexcept {
	*this = std::move(other);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
	_vertices = std::move(other._vertices);
	_indices = std::move(other._indices);
	_textures = std::move(other._textures);

	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
	_vao = other._vao;
	other._vao = 0;
	_vbo = other._vbo;
	other._vbo = 0;
	_ebo = other._ebo;
	other._ebo = 0;
	return *this;
}
