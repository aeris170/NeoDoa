#pragma once

#include <glm/glm.hpp>
#include <angelscript.h>
#include <scriptarray/scriptarray.h>

#include "Module.hpp"

struct Transform : Module {

	using Module::Module; // inherit ctors

	glm::vec3& Translation() const;
	glm::quat& Rotation() const;
	glm::vec3& Scale() const;
	bool& Selected();
	Transform*& Parent();
	CScriptArray& Children() const;

	void Adopt(Transform& child);
	void Disown(Transform& child);
};

glm::mat4 CreateModelMatrixFromTransform(asIScriptObject* transform);
