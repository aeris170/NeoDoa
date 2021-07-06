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
	DOA_LOG_ERROR("Module %s not found.", moduleType.data());
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

std::optional<std::reference_wrapper<Module>> ScriptComponent::TryGet(std::string_view moduleType) {
	int index = IndexOf(moduleType);
	if (index != -1) {
		return _modules[index];
	}
	return std::nullopt;
}

Module& ScriptComponent::Attach(std::string_view moduleType, bool callerIsEngine) {
	int index = IndexOf(moduleType);
	if (index != -1) {
		if (callerIsEngine) {
			DOA_LOG_TRACE("Didn't attach. Entity already has %s!", moduleType.data());
		} else {
			CLI_LOG_TRACE("Didn't attach. Entity already has %s!", moduleType.data());
		}
		return operator[](moduleType);
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
		return;
	}
	Module& module = _modules[index];
	if(module._isDef) {
		if (callerIsEngine) {
			DOA_LOG_TRACE("Didn't detach. %s is an essential module!", moduleType.data());
		}
		else {
			CLI_LOG_TRACE("Didn't detach. %s is an essential module!", moduleType.data());
		}
		return;
	}
	_modules.erase(_modules.begin() + index);
}
