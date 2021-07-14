#include "Transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core.hpp"
#include "Angel.hpp"

glm::vec3& Transform::Translation() const { return GetAt<glm::vec3>(1); }
glm::quat& Transform::Rotation() const { return GetAt<glm::quat>(2); }
glm::vec3& Transform::Scale() const { return GetAt<glm::vec3>(3); }
bool& Transform::Selected() { return GetAt<bool>(4); }
Transform*& Transform::Parent() { return GetAt<Transform*>(5); }
CScriptArray& Transform::Children() const { return GetAt<CScriptArray>(6); };

void Transform::Adopt(Transform& child) {
	static std::unique_ptr<Core>& core = GetCore();
	core->_angel->_scriptCtx->Prepare(_module->GetObjectType()->GetMethodByDecl("void Adopt(Transform@)"));
	core->_angel->_scriptCtx->SetObject(_module);
	core->_angel->_scriptCtx->SetArgObject(0, child._module);
	core->_angel->_scriptCtx->Execute();
}

void Transform::Disown(Transform& child) {
	static std::unique_ptr<Core>& core = GetCore();
	core->_angel->_scriptCtx->Prepare(_module->GetObjectType()->GetMethodByDecl("void Disown(Transform@)"));
	core->_angel->_scriptCtx->SetObject(_module);
	core->_angel->_scriptCtx->SetArgObject(0, child._module);
	core->_angel->_scriptCtx->Execute();
}

glm::mat4 CreateModelMatrixFromTransform(asIScriptObject* transformScript) {
	Transform transform("tmp", transformScript);
	Transform*& parent = transform.Parent();
	glm::mat4 model(1);
	while(parent != nullptr) {
		model = CreateModelMatrixFromTransform(*parent);
	}
	model = glm::translate(model, transform.Translation());
	model *= glm::toMat4(transform.Rotation());
	model = glm::scale(model, transform.Scale());
	return model;
}
