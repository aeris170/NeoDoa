#include "ScriptComponent.hpp"

#include "Log.hpp"

std::vector<PropertyData>& ScriptComponent::Properties() {
	return GetCore()->_angel->_scriptComponentData[_name];
}

const std::vector<PropertyData>& ScriptComponent::Properties() const {
	return GetCore()->_angel->_scriptComponentData[_name];
}

void* ScriptComponent::GetAddressOfPropertyAt(int index) const {
	if (_underlyingInstance == nullptr) {
		DOA_LOG_FATAL("Underlying instance for %s is NULL!", _name);
		return nullptr;
	}
	return _underlyingInstance->GetAddressOfProperty(index);
}
