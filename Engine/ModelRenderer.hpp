#pragma once

#include "Model.hpp"
#include "Shader.hpp"

struct ModelRenderer {

	Model*& Model() const;
	Shader*& Shader() const;

};
