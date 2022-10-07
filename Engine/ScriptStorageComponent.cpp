#include "ScriptStorageComponent.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Log.hpp"
#include "ScriptComponentData.hpp"

ScriptStorageComponent::ScriptStorageComponent(Entity owner) noexcept
    :_owner(owner) {}

ScriptComponent& ScriptStorageComponent::operator[](std::string_view componentType) {
    return _components[std::string(componentType)];
}

bool ScriptStorageComponent::TryGet(std::string_view componentType, ScriptComponent*& module) {
    auto search = _components.find(componentType);
    if (search != _components.end()) {
        module = &search->second;
        return true;
    }
    module = nullptr;
    return false;
}

ScriptComponent& ScriptStorageComponent::Attach(std::string_view componentType) {
    ScriptComponent* ptr = nullptr;
    if (TryGet(componentType, ptr)) {
        DOA_LOG_TRACE("Didn't attach. Entity already has %s!", ptr->_name);
        return *ptr;
    } else {
        ScriptComponent c{ Core::GetCore()->Angel()->InstantiateScriptComponentIncomplete(componentType) };
        c._storage = this;
        return _components.emplace(std::string(componentType), c).first->second;
    }
}

ScriptComponent& ScriptStorageComponent::AttachWithData(std::string_view componentType, const ScriptComponentData& data) {
    if (componentType != data.name) {
        DOA_LOG_WARNING("Didn't attach with data. Data for %s is not convertible to %s", data.name, componentType);
    }

    ScriptComponent& script = Attach(componentType);

    const std::vector<PropertyData>& props = script.Properties();
    if (props.size() != data.data.size()) {
        DOA_LOG_WARNING("Did attach with data, but may crash. Data for %s may not be compatible with %s (version mismatch?)", data.name, componentType);
    }

    for (auto i = 0; i < props.size(); i++) {
        const PropertyData& prop = props[i];
        auto it = Core::GetCore()->Angel()->_enums.find(prop.typeName);
        if (it != Core::GetCore()->Angel()->_enums.end()) script.GetAt<int>(i) = std::any_cast<int>(data.data[i]);
        else if (prop.typeName == "Entity")        script.GetAt<Entity>(i) = std::any_cast<Entity>(data.data[i]);
        else if (prop.typeName == "int")           script.GetAt<int>(i) = std::any_cast<int>(data.data[i]);
        else if (prop.typeName == "float")         script.GetAt<float>(i) = std::any_cast<float>(data.data[i]);
        else if (prop.typeName == "double")        script.GetAt<double>(i) = std::any_cast<double>(data.data[i]);
        else if (prop.typeName == "string")        script.GetAt<std::string>(i) = std::any_cast<std::string>(data.data[i]);
        else if (prop.typeName == "vec2")          script.GetAt<glm::vec2>(i) = std::any_cast<glm::vec2>(data.data[i]);
        else if (prop.typeName == "vec3")          script.GetAt<glm::vec3>(i) = std::any_cast<glm::vec3>(data.data[i]);
        else if (prop.typeName == "vec4")          script.GetAt<glm::vec4>(i) = std::any_cast<glm::vec4>(data.data[i]);
        else if (prop.typeName == "quat")          script.GetAt<glm::quat>(i) = std::any_cast<glm::quat>(data.data[i]);
        else if (prop.typeName == "mat2")          script.GetAt<glm::mat2>(i) = std::any_cast<glm::mat2>(data.data[i]);
        else if (prop.typeName == "mat3")          script.GetAt<glm::mat3>(i) = std::any_cast<glm::mat3>(data.data[i]);
        else if (prop.typeName == "mat4")          script.GetAt<glm::mat4>(i) = std::any_cast<glm::mat4>(data.data[i]);
        else {
            if (prop.isReference) {
                // TODO
                DOA_LOG_TRACE("prop.isReference");
            } else {
                // TODO
                DOA_LOG_TRACE("!prop.isReference");
            }
        }
    }

    return script;
}

void ScriptStorageComponent::Detach(std::string_view componentType) {
    ScriptComponent* ptr = nullptr;
    if (!TryGet(componentType, ptr)) {
        DOA_LOG_TRACE("Didn't attach. %s doesn't exist!", ptr->_name);
    } else {
        _components.erase(ptr->_name);
    }
}