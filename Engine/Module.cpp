#include "Module.hpp"

#include "Core.hpp"
#include "Angel.hpp"

Module::Module(std::string_view name, asIScriptObject* module) noexcept :
    _name(name),
    _module(module) {
    _module->AddRef();
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
    return *this;
}

EntityID Module::GetID() const {
    auto* entity = (asIScriptObject*)_module->GetAddressOfProperty(0);
    return EntityID(*(int*)entity->GetAddressOfProperty(0));
}

void Module::SetID(int id) {
    auto* entity = (asIScriptObject*)_module->GetAddressOfProperty(0);
    *(int*)entity->GetAddressOfProperty(0) = id;
}

void Module::SetID(EntityID id) {
    SetID(static_cast<int>(id));
}
