#pragma once

#include <string>

#include "Angel.hpp"

struct ScriptStorageComponent;
class asIScriptObject;

struct ScriptComponent {

    std::string _name;
    ScriptStorageComponent* _storage;
    asIScriptObject* _underlyingInstance{ nullptr };

    std::vector<PropertyData>& Properties();
    const std::vector<PropertyData>& Properties() const;

    void* GetAddressOfPropertyAt(int index) const;

    template<typename T>
    T& As() {
        return static_cast<T&>(*this);
    }

    template<typename T>
    T& GetAt(int index) const {
        return *static_cast<T*>(GetAddressOfPropertyAt(index));
    }
};