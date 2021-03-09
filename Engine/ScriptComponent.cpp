#include "ScriptComponent.hpp"

#include <angelscript.h>

#include "Log.hpp"
#include "Core.hpp"
#include "Angel.hpp"

ScriptComponent::ScriptComponent(int id) noexcept :
	_id(id) {}

ScriptComponent::~ScriptComponent() noexcept {}

ScriptComponent::ScriptComponent(ScriptComponent&& other) noexcept {
	*this = std::move(other);
}

ScriptComponent& ScriptComponent::operator=(ScriptComponent&& other) noexcept {
	std::swap(_id, other._id);
	_modules.swap(other._modules);
	return *this;
}

Module& ScriptComponent::operator[](int index) {
	if (index > -1 && index < _modules.size()) {
		return _modules[index];
	}
	DOA_LOG_ERROR("Index out of range");
	throw;
}

Module& ScriptComponent::operator[](std::string_view moduleType) {
	int index = IndexOf(moduleType);
	if (index != -1) {
		return _modules[index];
	}
	DOA_LOG_ERROR("Module not found");
	throw;
}

int ScriptComponent::IndexOf(std::string_view moduleType) {
	int i = 0;
	for (const auto& module : _modules) {
		if (module._name == moduleType) {
			return i;
		}
		i++;
	}
	return -1;
}

Module& ScriptComponent::Get(std::string_view moduleType) {
	int index = IndexOf(moduleType);
	if (index != -1) {
		return _modules[index];
	}
	DOA_LOG_TRACE("Cannot find component %s. Returned nullptr!", moduleType.data());
	throw;
}

Module& ScriptComponent::Attach(std::string_view moduleType, bool callerIsEngine) {
	int index = IndexOf(moduleType);
	if (index != -1) {
		if (callerIsEngine) {
			DOA_LOG_TRACE("Didn't attach. Component already has %s!", moduleType.data());
		} else {
			CLI_LOG_TRACE("Didn't attach. Component already has %s!", moduleType.data());
		}
		return Get(moduleType);
	}
	return _modules.emplace_back(GetCore()->_angel->InstantiateModule(moduleType, _id));
}

void ScriptComponent::Detach(std::string_view moduleType, bool callerIsEngine) {
	int index = IndexOf(moduleType);
	if (index == -1) {
		if(callerIsEngine) {
			DOA_LOG_TRACE("Didn't detach. %s not present!", moduleType.data());
		} else {
			CLI_LOG_TRACE("Didn't detach. %s not present!", moduleType.data());
		}
	}
	_modules.erase(_modules.begin() + index);
}
