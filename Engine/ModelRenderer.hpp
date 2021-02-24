#pragma once

#include "Module.hpp"

#include "Model.hpp"
#include "Shader.hpp"

struct ModelRenderer : Module {

	using Module::Module; // inherit ctors

	Model*& Model() const;
	Shader*& Shader() const;

};
