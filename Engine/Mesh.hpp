#pragma once

#include <vector>

#include <GL/glew.h>

#include "Vertex.hpp"
#include "TypedefsAndConstants.hpp"

struct Mesh {
	Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices) noexcept;
	~Mesh() noexcept;
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept;

	VAO VertexArrayObject() const;
	bool HasIndexBuffer() const;
	EBO ElementBufferObject() const;

	std::vector<Vertex>& Vertices();
	const std::vector<Vertex>& Vertices() const;

	std::vector<GLuint>& Indices();
	const std::vector<GLuint>& Indices() const;

	void DeAlloc();
	void UpdateVRAM();

private:
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;

	VAO _vao{ 0 };
	VBO _vbo{ 0 };
	EBO _ebo{ 0 };
};
