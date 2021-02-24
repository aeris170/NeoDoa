#pragma once

#include <angelscript.h>

#include "TypedefsAndConstants.hpp"

struct Module {

	std::string _name;
	asIScriptObject* _module{ nullptr };

	Module(std::string_view name, asIScriptObject* module) noexcept;
	~Module() noexcept;
	Module(const Module&) = delete;
	Module(Module&&) noexcept;
	Module& operator=(const Module&) = delete;
	Module& operator=(Module&&) noexcept;

	operator EntityID() const { return GetID(); }
	operator asIScriptObject*() const { return _module; }

	EntityID GetID() const;
	void SetID(int id);
	void SetID(EntityID id);

	template<typename T>
	T& As() {
		return static_cast<T&>(*this);
	}

	template<typename T>
	T& GetAt(int index) const {
		return *static_cast<T*>(_module->GetAddressOfProperty(index));
	}
};

