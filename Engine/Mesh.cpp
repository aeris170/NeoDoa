#include "Mesh.hpp"

#include <cstddef>

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, std::vector<std::weak_ptr<Texture>>&& textures) noexcept :
	_vertices(std::move(vertices)),
	_indices(std::move(indices)),
	_textures(std::move(textures)) {
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	auto vertexCount = _vertices.size();
	auto vertexSize = sizeof(Vertex);

	auto totalSize = vertexCount * vertexSize;

	glBufferData(GL_ARRAY_BUFFER, totalSize, _vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(5);

	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)offsetof(Vertex, color));
	glEnableVertexAttribArray(6);

	glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(7);

	glVertexAttribIPointer(8, 1, GL_SHORT, vertexSize, (GLvoid*)offsetof(Vertex, texIndex));
	glEnableVertexAttribArray(8);

	if (_indices.size() > 0) {
		glGenBuffers(1, &_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
