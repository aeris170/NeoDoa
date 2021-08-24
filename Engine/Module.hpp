#pragma once

#include <any>

#include <angelscript.h>

#include "PropertyData.hpp"
#include "TypedefsAndConstants.hpp"

struct Module {

	std::string _name;
	bool _isActive;

	asIScriptObject* _module{ nullptr };
	bool _isDef{ false };

	Module(std::string_view name, asIScriptObject* module) noexcept;
	~Module() noexcept;
	Module(const Module&) = delete;
	Module(Module&&) noexcept;
	Module& operator=(const Module&) = delete;
	Module& operator=(Module&&) noexcept;

	operator EntityID() const { return GetID(); }
	operator asIScriptObject* () const { return _module; }

	EntityID GetID() const;
	void SetID(int id);
	void SetID(EntityID id);

	std::vector<PropertyData>& Properties();

	void* GetAddressOfPropertyAt(int index);

#ifdef EDITOR
	void BeforePropertiesGUI();
	bool OnDrawPropertyGUI(PropertyData& property, int propertyIndex);
	void AfterPropertiesGUI();
#endif

	template<typename T>
	T& As() {
		return static_cast<T&>(*this);
	}

	template<typename T>
	T& GetAt(int index) const {
		return *static_cast<T*>(_module->GetAddressOfProperty(index));
	}
};
