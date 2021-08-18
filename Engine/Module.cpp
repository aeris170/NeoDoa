#include "Module.hpp"

#include "Core.hpp"
#include "Angel.hpp"

Module::Module(std::string_view name, asIScriptObject* module) noexcept :
    _name(name),
    _module(module) {
    if(_module) {
        _module->AddRef();
    }
}

Module::~Module() noexcept {
    if (_module) _module->Release();
}

Module::Module(Module&& other) noexcept {
    *this = std::move(other);
}

Module& Module::operator=(Module&& other) noexcept {
    _name = std::move(other._name);
    if (_module) _module->Release();
    std::swap(_module, other._module);
    _isDef = other._isDef;
    return *this;
}

EntityID Module::GetID() const {
    if(_module) {
        auto* entity = static_cast<asIScriptObject*>(_module->GetAddressOfProperty(0));
        return EntityID(*static_cast<int*>(entity->GetAddressOfProperty(0)));
    }
    return NULL_ENTT;
}

void Module::SetID(int id) {
    if (!_module) {
        return;
    }
    auto* entity = static_cast<asIScriptObject*>(_module->GetAddressOfProperty(0));
    *static_cast<int*>(entity->GetAddressOfProperty(0)) = id;
}

void Module::SetID(EntityID id) {
    SetID(static_cast<int>(id));
}

std::vector<PropertyData>& Module::Properties() {
    return GetCore()->_angel->_modules[_module->GetObjectType()->GetName()];
}

void* Module::GetAddressOfPropertyAt(int index) {
    return _module->GetAddressOfProperty(index);
}

#ifdef EDITOR
void Module::BeforePropertiesGUI(){
    GetCore()->_angel->CallModuleFunction(*this, "BeforePropertiesGUI", {}, {});
}

bool Module::OnDrawPropertyGUI(PropertyData& property, int propertyIndex) {
    //if (property.isPrivate || property.isProtected) { return false; } // don't show private or protected variables in editor!
    return *(bool*)GetCore()->_angel->CallModuleFunction(*this, "OnDrawPropertyGUI", { "PropertyData" , "int" }, { &property, &propertyIndex });
}

void Module::AfterPropertiesGUI() {
    GetCore()->_angel->CallModuleFunction(*this, "AfterPropertiesGUI", {}, {});
}
#endif