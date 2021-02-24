#pragma once

#include <vector>

#include <GL/glew.h>

#include "Vertex.hpp"
#include "TypedefsAndConstants.hpp"

struct Texture;

struct Mesh {
	std::vector<Vertex> _vertices;
	std::vector<GLuint> _indices;
	std::vector<std::weak_ptr<Texture>> _textures; // TODO mesh shouldn't have this kind of data.. move this when you implement materials!

	VAO _vao{ 0 };
	VBO _vbo{ 0 };
	EBO _ebo{ 0 };

	Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, std::vector<std::weak_ptr<Texture>>&& textures) noexcept;
	~Mesh() noexcept;
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept;
};
