#include "ModelRenderer.hpp"

typedef Model* ModelPointer;
typedef Shader* ShaderPointer;

ModelPointer& ModelRenderer::Model() const { return GetAt<ModelPointer>(1); }
ShaderPointer& ModelRenderer::Shader() const { return GetAt<ShaderPointer>(2); }
