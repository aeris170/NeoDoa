#pragma once

#include <string>
#include <utility>
#include <optional>
#include <unordered_map>

#include "Module.hpp"
#include "TypedefsAndConstants.hpp"

class asIScriptObject;

struct ScriptComponent {

	int _id;
	std::vector<Module> _modules;

	ScriptComponent(int id) noexcept;
	~ScriptComponent() noexcept;
	ScriptComponent(const ScriptComponent&) = delete;
	ScriptComponent(ScriptComponent&&) noexcept;
	ScriptComponent& operator=(const ScriptComponent&) = delete;
	ScriptComponent& operator=(ScriptComponent&&) noexcept;

	Module& operator[](int index);
	Module& operator[](std::string_view componentType);

	int IndexOf(std::string_view componentType);
	std::optional<std::reference_wrapper<Module>> TryGet(std::string_view componentType);
	Module& Attach(std::string_view componentType, bool callerIsEngine = true);
	void Detach(std::string_view componentType, bool callerIsEngine = true);
};
