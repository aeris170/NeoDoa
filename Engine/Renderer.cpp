#include "Renderer.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Model.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

#include "OrthoCamera.hpp"
#include "PerspectiveCamera.hpp"

#include "Texture.hpp"
#include "Core.hpp"
#include "ScriptComponent.hpp"
#include "Transform.hpp"

#include "Log.hpp"

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
				prevVbo = 0; // previous will get wiped, null prevVbo so it stays alive! :)
				return;
			}
		}
	}

	// if we are here, didn't return in previous if clause, therefore we need a new vbo for our mesh
	auto v4s = sizeof(glm::vec4);
	auto m4s = sizeof(glm::mat4);

	VBO vbo;
	glCreateBuffers(1, &vbo);
	glNamedBufferData(vbo, defaultSize * m4s, nullptr, GL_DYNAMIC_DRAW);

	for (auto& mesh : model.lock()->_meshes) {
		glVertexArrayVertexBuffer(mesh._vao, 1, vbo, 0, m4s);

		glEnableVertexArrayAttrib(mesh._vao, 0);
		glEnableVertexArrayAttrib(mesh._vao, 1);
		glEnableVertexArrayAttrib(mesh._vao, 2);
		glEnableVertexArrayAttrib(mesh._vao, 3);

		glVertexArrayAttribBinding(mesh._vao, 0, 1);
		glVertexArrayAttribBinding(mesh._vao, 1, 1);
		glVertexArrayAttribBinding(mesh._vao, 2, 1);
		glVertexArrayAttribBinding(mesh._vao, 3, 1);

		glVertexArrayAttribFormat(mesh._vao, 0, 4, GL_FLOAT, GL_FALSE, v4s * 0);
		glVertexArrayAttribFormat(mesh._vao, 1, 4, GL_FLOAT, GL_FALSE, v4s * 1);
		glVertexArrayAttribFormat(mesh._vao, 2, 4, GL_FLOAT, GL_FALSE, v4s * 2);
		glVertexArrayAttribFormat(mesh._vao, 3, 4, GL_FLOAT, GL_FALSE, v4s * 3);

		glVertexArrayBindingDivisor(mesh._vao, 1, 1);
	}

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

void Renderer::Render(entt::registry& registry, ACamera* cam) {
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
		auto& shader = pair.first;
		shader->Use();
		shader->UniformMat4("viewProjMatrix", glm::value_ptr(cam->_viewProjectionMatrix));
		for (auto& tuple : pair.second) {
			auto& [model, instances, size, vbo] = tuple;
			auto modelPtr = model.lock();

			auto v4s = sizeof(glm::vec4);
			auto m4s = sizeof(glm::mat4);
			if (instances.size() > size) {
				// need to realloc! find new size
				while (size <= instances.size()) {
					size *= defaultGrowthFactor;
				}
				// realloc!
				glNamedBufferData(vbo, size * m4s, nullptr, GL_DYNAMIC_DRAW);
			}

			std::vector<glm::mat4> transforms;
			transforms.reserve(size);
			for (auto entity : instances) {
				transforms.emplace_back(CreateModelMatrixFromTransform(registry.get<ScriptComponent>(entity)["Transform"]));
			}
			glNamedBufferSubData(vbo, 0, transforms.size() * m4s, transforms.data());

			for (auto& mesh : modelPtr->_meshes) {
				FindTexture("!!missing!!").value().lock()->Bind(); // override possible overrides to missing texture.
				int i = 1;
				for (auto& tex : mesh._textures) {
					if (i == 17) {
						DOA_LOG_WARNING("Model %s has too many textures! Rendering may look incorrect.", modelPtr->_name);
						break; //too many textures...
					}
					tex.lock()->Bind(i++);
				}

				glBindVertexArray(mesh._vao);
				if (mesh._ebo > 0) {
					glDrawElementsInstanced(GL_TRIANGLES, mesh._indices.size(), GL_UNSIGNED_INT, 0, instances.size());
				} else {
					glDrawArraysInstanced(GL_TRIANGLES, 0, mesh._vertices.size(), instances.size());
				}
				glBindVertexArray(0);

				vertices += mesh._vertices.size();
				indices += mesh._indices.size();
				drawCalls++;
			}
		}
	}

	_previous = std::move(_current);
	_current.clear(); // _current is in an undefined state, clear() brings it back to a defined one
}