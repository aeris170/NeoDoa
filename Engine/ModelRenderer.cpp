#include "ModelRenderer.hpp"

typedef Model* ModelPointer;
typedef Shader* ShaderPointer;

Model*& ModelRenderer::Model() const { return GetAt<ModelPointer>(1); }
Shader*& ModelRenderer::Shader() const { return GetAt<ShaderPointer>(2); }
