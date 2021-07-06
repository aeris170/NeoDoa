#include <iostream>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.hpp"

#include "Model.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

#include "OrthoCamera.hpp"
#include "PerspectiveCamera.hpp"

#include "Texture.hpp"
#include "Core.hpp"
#include "ScriptComponent.hpp"
#include "Transform.hpp"

static constexpr int defaultSize = 10;
static constexpr int defaultGrowthFactor = 2;

Renderer::Renderer() noexcept {}

Renderer::~Renderer() noexcept {}

//-----------------------------------------------------------------

void Renderer::Register(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model) {
	auto ptr = shader.lock();
	ptr->Use();
	auto it = _current.find(ptr);
	if (it != _current.end()) {
		auto modelPtr = model.lock();
		for (auto& tuple : it->second) {
			auto& [model, instances, size, vbo] = tuple;
			if (modelPtr == model.lock()) {
				return;
			}
		}
	}

	it = _previous.find(ptr);
	std::vector<EntityID> vec;
	if (it != _previous.end()) {
		for (auto& tuple : it->second) {
			auto& [prevModel, prevInstance, prevSize, prevVbo] = tuple;
			if (prevModel.lock() == model.lock()) {
				vec.reserve(prevSize);
				_current[ptr].emplace_back(std::make_tuple(model, vec, prevSize, prevVbo));
				prevVbo = 0;
				return;
			}
		}
	}

	auto v4s = sizeof(glm::vec4);
	auto m4s = sizeof(glm::mat4);

	VBO vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, defaultSize * m4s, nullptr, GL_DYNAMIC_DRAW);

	for (auto& mesh : model.lock()->_meshes) {
		glBindVertexArray(mesh._vao);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, m4s, 0);
		glEnableVertexAttribArray(0);
		glVertexAttribDivisor(0, 1);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, m4s, (void*)v4s);
		glEnableVertexAttribArray(1);
		glVertexAttribDivisor(1, 1);

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, m4s, (void*)(v4s * 2));
		glEnableVertexAttribArray(2);
		glVertexAttribDivisor(2, 1);

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, m4s, (void*)(v4s * 3));
		glEnableVertexAttribArray(3);
		glVertexAttribDivisor(3, 1);
	}
	glBindVertexArray(0);

	vec.reserve(defaultSize);
	_current[ptr].emplace_back(std::make_tuple(model, vec, defaultSize, vbo));
}

void Renderer::Submit(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model, EntityID instance) {
	auto modelPtr = model.lock();
	auto& vector = _current.at(shader.lock());
	for (auto& tuple : vector) {
		auto& [model, instances, size, vbo] = tuple;
		if (modelPtr == model.lock()) {
			instances.push_back(instance);
			return;
		}
	}
}

void Renderer::Render(entt::registry& registry, Camera* cam) {
	vertices = 0;
	indices = 0;
	drawCalls = 0;

	std::vector<VBO> toBeDeleted;
	for (auto& pair : _previous) {
		for (auto& element : pair.second) {
			toBeDeleted.push_back(std::get<VBO>(element));
		}
	}
	glDeleteBuffers(toBeDeleted.size(), toBeDeleted.data());
	// done
	for (auto& pair : _current) {
		auto shader = pair.first;
		shader->Use();
		shader->UniformMat4("viewProjMatrix", glm::value_ptr(cam->_viewProjectionMatrix));
		for (auto& tuple : pair.second) {
			auto& [model, instances, size, vbo] = tuple;
			auto modelPtr = model.lock();

			auto v4s = sizeof(glm::vec4);
			auto m4s = sizeof(glm::mat4);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			if (instances.size() > size) {
				while (size <= instances.size()) {
					size *= defaultGrowthFactor;
				}
				glBufferData(GL_ARRAY_BUFFER, size * m4s, nullptr, GL_DYNAMIC_DRAW);

				for (auto& mesh : modelPtr->_meshes) {
					glBindVertexArray(mesh._vao);
					glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, m4s, 0);
					glEnableVertexAttribArray(0);
					glVertexAttribDivisor(0, 1);

					glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, m4s, (void*)v4s);
					glEnableVertexAttribArray(1);
					glVertexAttribDivisor(1, 1);

					glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, m4s, (void*)(v4s * 2));
					glEnableVertexAttribArray(2);
					glVertexAttribDivisor(2, 1);

					glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, m4s, (void*)(v4s * 3));
					glEnableVertexAttribArray(3);
					glVertexAttribDivisor(3, 1);
				}
			} else {
				std::vector<glm::mat4> transforms;
				transforms.reserve(size);
				for (auto entity : instances) {
					transforms.emplace_back(CreateModelMatrixFromTransform(registry.get<ScriptComponent>(entity)["Transform"]));
				}
				glBufferSubData(GL_ARRAY_BUFFER, 0, transforms.size() * m4s, transforms.data());
			}

			for (auto& mesh : modelPtr->_meshes) {
				glBindVertexArray(mesh._vao);
				shader->Uniformi("missing", 0);
				shader->Uniformi("texture1", 1);
				shader->Uniformi("texture2", 2);
				shader->Uniformi("texture3", 3);
				shader->Uniformi("texture4", 4);
				shader->Uniformi("texture5", 5);
				shader->Uniformi("texture6", 6);
				shader->Uniformi("texture7", 7);
				shader->Uniformi("texture8", 8);
				shader->Uniformi("texture9", 9);
				shader->Uniformi("texture10", 10);
				shader->Uniformi("texture11", 11);
				shader->Uniformi("texture12", 12);
				shader->Uniformi("texture13", 13);
				shader->Uniformi("texture14", 14);
				shader->Uniformi("texture15", 15);
				shader->Uniformi("texture16", 16);
				FindTexture("!!missing!!").value().lock()->Bind();
				int i = 1;
				for (auto& tex : mesh._textures) {
					if (i == 17) break; //too many textures...
					tex.lock()->Bind(i++);
				}
				if (mesh._ebo > 0) {
					glDrawElementsInstanced(GL_TRIANGLES, mesh._indices.size(), GL_UNSIGNED_INT, 0, instances.size());
				} else {
					glDrawArraysInstanced(GL_TRIANGLES, 0, mesh._vertices.size(), instances.size());
				}
				vertices += mesh._vertices.size();
				indices += mesh._indices.size();
				drawCalls++;
			}
		}
	}

	_previous = std::move(_current);
	_current.clear(); // _current is in an undefined state, clear() brings it back to a defined one
}