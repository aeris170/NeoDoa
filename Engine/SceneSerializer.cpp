#include "SceneSerializer.hpp"

#include <nameof.hpp>

#include "Scene.hpp"
#include "IDComponent.hpp"
#include "TransformComponent.hpp"
#include "ParentComponent.hpp"
#include "ChildComponent.hpp"
#include "CameraComponent.hpp"

std::string SerializeScene(const Scene& scene) {
    tinyxml2::XMLPrinter printer;
    SceneSerializer::HeaderComment(printer, scene);
    SceneSerializer::Serialize(printer, scene);

    return printer.CStr();
}

void SceneSerializer::DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const Scene& scene) {
    printer.PushComment("WARNING!! This file is not for editing! Don't!");
}
void SceneSerializer::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene) {
    printer.OpenElement("scene");
    {
        SceneConfig::Serialize(printer, scene);
        Entities::Serialize(printer, scene);
    }
    printer.CloseElement();
}

void SceneSerializer::SceneConfig::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene) {
    printer.OpenElement("config");
    {
        SerializeName(printer, scene.Name);
        SerializeClearColor(printer, scene.ClearColor);
        SerializeSelectionOutlineColor(printer, scene.SelectionOutlineColor);
        Cameras::Serialize(printer, scene);
    }
    printer.CloseElement(); // config close
}
void SceneSerializer::SceneConfig::DefaultSerializeName(tinyxml2::XMLPrinter& printer, ::std::string_view name) {
    printer.PushAttribute("name", name.data());
}
void SceneSerializer::SceneConfig::DefaultSerializeClearColor(tinyxml2::XMLPrinter& printer, const Color& color) {
    printer.OpenElement("clearColor");
    printer.PushAttribute("r", color.r);
    printer.PushAttribute("g", color.g);
    printer.PushAttribute("b", color.b);
    printer.CloseElement();
}
void SceneSerializer::SceneConfig::DefaultSerializeSelectionOutlineColor(tinyxml2::XMLPrinter& printer, const Color& color){
    printer.OpenElement("selectionOutlineColor");
    printer.PushAttribute("r", color.r);
    printer.PushAttribute("g", color.g);
    printer.PushAttribute("b", color.b);
    printer.CloseElement();
}

void SceneSerializer::SceneConfig::Cameras::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene) {
    printer.OpenElement("cameras");
    {
        SerializeActiveCamera(printer, scene);
        SerializeOrthoCamera(printer, scene.GetOrtho());
        SerializePerspectiveCamera(printer, scene.GetPerspective());
    }
    printer.CloseElement();
}
void SceneSerializer::SceneConfig::Cameras::DefaultSerializeActiveCamera(tinyxml2::XMLPrinter& printer, const Scene& scene) {
    printer.OpenElement("activeCamera");
    {
        if (scene.IsOrtho()) {
            printer.PushAttribute("type", "ortho");
        } else if (scene.IsPerspective()) {
            printer.PushAttribute("type", "perspective");
        } else {
            assert(false); /* no camera? */
            throw 1;
        }

        const ACamera& activeCamera = scene.GetActiveCamera();
        printer.OpenElement("eye");
        printer.PushAttribute("x", activeCamera.eye.x);
        printer.PushAttribute("y", activeCamera.eye.y);
        printer.PushAttribute("z", activeCamera.eye.z);
        printer.CloseElement();

        printer.OpenElement("forward");
        printer.PushAttribute("x", activeCamera.forward.x);
        printer.PushAttribute("y", activeCamera.forward.y);
        printer.PushAttribute("z", activeCamera.forward.z);
        printer.CloseElement();

        printer.OpenElement("up");
        printer.PushAttribute("x", activeCamera.up.x);
        printer.PushAttribute("y", activeCamera.up.y);
        printer.PushAttribute("z", activeCamera.up.z);
        printer.CloseElement();

        printer.OpenElement("zoom");
        printer.PushAttribute("value", activeCamera.zoom);
        printer.CloseElement();
    }
    printer.CloseElement();
}
void SceneSerializer::SceneConfig::Cameras::DefaultSerializeOrthoCamera(tinyxml2::XMLPrinter& printer, const OrthoCamera& camera) {
    printer.OpenElement("orthoCamera");
    {
        printer.PushAttribute("left", camera._left);
        printer.PushAttribute("right", camera._right);
        printer.PushAttribute("bottom", camera._bottom);
        printer.PushAttribute("top", camera._top);
        printer.PushAttribute("near", camera._near);
        printer.PushAttribute("far", camera._far);
    }
    printer.CloseElement();
}
void SceneSerializer::SceneConfig::Cameras::DefaultSerializePerspectiveCamera(tinyxml2::XMLPrinter& printer, const PerspectiveCamera& camera) {
    printer.OpenElement("perspectiveCamera");
    {
        printer.PushAttribute("fov", camera._fov);
        printer.PushAttribute("aspect", camera._aspect);
        printer.PushAttribute("near", camera._near);
        printer.PushAttribute("far", camera._far);
    }
    printer.CloseElement();
}

void SceneSerializer::Entities::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene) {
    printer.OpenElement("entities");
    {
        for (auto& entity : scene.GetAllEntites()) {
            SerializeEntity(printer, scene, entity);
        }
    }
    printer.CloseElement();
}
void SceneSerializer::Entities::DefaultSerializeEntity(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity) {
    printer.OpenElement("entity");
    {
        SerializeIDComponent(printer, scene.GetComponent<IDComponent>(entity));
        SerializeTransformComponent(printer, scene.GetComponent<TransformComponent>(entity));
        if (scene.HasComponent<ParentComponent>(entity)) {
            SerializeParentComponent(printer, scene.GetComponent<ParentComponent>(entity));
        }
        if (scene.HasComponent<ChildComponent>(entity)) {
            SerializeChildComponent(printer, scene.GetComponent<ChildComponent>(entity));
        }
        SerializeUserDefinedComponents(printer, scene, entity);
    }
    printer.CloseElement();
}
void SceneSerializer::Entities::DefaultSerializeIDComponent(tinyxml2::XMLPrinter& printer, const IDComponent& component) {
    printer.OpenElement("cpp-component");
    {
        printer.PushAttribute("name", nameof_c(IDComponent));

        auto name = nameof(IDComponent::entity);
        printer.OpenElement(name.data());
        SceneSerializer::Helpers::SerializeEntityID(printer, component.GetEntity());
        printer.CloseElement();

        name = nameof(IDComponent::tag);
        printer.OpenElement(name.data());
        printer.PushAttribute("type", "string");
        printer.PushAttribute("value", component.GetTag().data());
        printer.CloseElement();
    }
    printer.CloseElement();
}
void SceneSerializer::Entities::DefaultSerializeTransformComponent(tinyxml2::XMLPrinter& printer, const TransformComponent& component) {
    printer.OpenElement("cpp-component");
    {
        printer.PushAttribute("name", nameof_c(TransformComponent));

        auto name = nameof(TransformComponent::localMatrix);
        printer.OpenElement(name.data());
        SceneSerializer::Helpers::SerializeMat4(printer, component.GetLocalMatrix());
        printer.CloseElement();
    }
    printer.CloseElement();
}
void SceneSerializer::Entities::DefaultSerializeParentComponent(tinyxml2::XMLPrinter& printer, const ParentComponent& component) {
    printer.OpenElement("cpp-component");
    {
        printer.PushAttribute("name", nameof_c(ParentComponent));

        auto name = nameof(ParentComponent::children);
        printer.OpenElement(name.data());
        {
            printer.PushAttribute("type", "vector");
            printer.OpenElement("values");
            {
                const auto& aux = component.GetChildren();
                int i = 0;
                for (auto& item : aux) {
                    printer.OpenElement("item");
                    printer.PushAttribute("index", i);
                    printer.PushAttribute("value", static_cast<uint32_t>(item));
                    printer.CloseElement();
                    i++;
                }
            }
            printer.CloseElement();
        }
        printer.CloseElement();
    }
    printer.CloseElement();
}
void SceneSerializer::Entities::DefaultSerializeChildComponent(tinyxml2::XMLPrinter& printer, const ChildComponent& component) {
    printer.OpenElement("cpp-component");
    {
        printer.PushAttribute("name", nameof_c(ChildComponent));

        auto name = nameof(ChildComponent::parent);
        printer.OpenElement(name.data());
        SceneSerializer::Helpers::SerializeEntityID(printer, component.GetParent());
        printer.CloseElement();
    }
    printer.CloseElement();
}
void SceneSerializer::Entities::DefaultSerializeUserDefinedComponents(tinyxml2::XMLPrinter&, const Scene&, Entity) {}

void SceneSerializer::Helpers::SerializeEntityID(tinyxml2::XMLPrinter& printer, Entity entity) {
    printer.PushAttribute("type", "Entity");
    printer.PushAttribute("id", static_cast<uint32_t>(entity));
}
void SceneSerializer::Helpers::SerializeEnum(tinyxml2::XMLPrinter& printer, std::string_view name, int value) {
    printer.PushAttribute("type", name.data());
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeInt(tinyxml2::XMLPrinter& printer, int value) {
    printer.PushAttribute("type", "int");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeFloat(tinyxml2::XMLPrinter& printer, float value) {
    printer.PushAttribute("type", "float");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeDouble(tinyxml2::XMLPrinter& printer, double value) {
    printer.PushAttribute("type", "double");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeString(tinyxml2::XMLPrinter& printer, std::string_view value) {
    printer.PushAttribute("type", "string");
    printer.PushAttribute("value", value.data());
}
void SceneSerializer::Helpers::SerializeVec2(tinyxml2::XMLPrinter& printer, const glm::vec2& vec) {
    printer.PushAttribute("type", "vec2");
    printer.PushAttribute("x", vec.x);
    printer.PushAttribute("y", vec.y);
}
void SceneSerializer::Helpers::SerializeVec3(tinyxml2::XMLPrinter& printer, const glm::vec3& vec) {
    printer.PushAttribute("type", "vec3");
    printer.PushAttribute("x", vec.x);
    printer.PushAttribute("y", vec.y);
    printer.PushAttribute("z", vec.z);
}
void SceneSerializer::Helpers::SerializeVec4(tinyxml2::XMLPrinter& printer, const glm::vec4& vec) {
    printer.PushAttribute("type", "vec4");
    printer.PushAttribute("x", vec.x);
    printer.PushAttribute("y", vec.y);
    printer.PushAttribute("z", vec.z);
    printer.PushAttribute("w", vec.w);
}
void SceneSerializer::Helpers::SerializeQuat(tinyxml2::XMLPrinter& printer, const glm::quat& quat) {
    printer.PushAttribute("type", "quat");
    printer.PushAttribute("w", quat.w);
    printer.PushAttribute("x", quat.x);
    printer.PushAttribute("y", quat.y);
    printer.PushAttribute("z", quat.z);
}
void SceneSerializer::Helpers::SerializeMat2(tinyxml2::XMLPrinter& printer, const glm::mat2& mat) {
    printer.PushAttribute("type", "mat2");

    printer.PushAttribute("x1", mat[0][0]);
    printer.PushAttribute("y1", mat[0][1]);

    printer.PushAttribute("x2", mat[1][0]);
    printer.PushAttribute("y2", mat[1][1]);
}
void SceneSerializer::Helpers::SerializeMat3(tinyxml2::XMLPrinter& printer, const glm::mat3& mat) {
    printer.PushAttribute("type", "mat3");

    printer.PushAttribute("x1", mat[0][0]);
    printer.PushAttribute("y1", mat[0][1]);
    printer.PushAttribute("z1", mat[0][2]);

    printer.PushAttribute("x2", mat[1][0]);
    printer.PushAttribute("y2", mat[1][1]);
    printer.PushAttribute("z2", mat[1][2]);

    printer.PushAttribute("x3", mat[2][0]);
    printer.PushAttribute("y3", mat[2][1]);
    printer.PushAttribute("z3", mat[2][2]);

}
void SceneSerializer::Helpers::SerializeMat4(tinyxml2::XMLPrinter& printer, const glm::mat4& mat) {
    printer.PushAttribute("type", "mat4");

    printer.PushAttribute("x1", mat[0][0]);
    printer.PushAttribute("y1", mat[0][1]);
    printer.PushAttribute("z1", mat[0][2]);
    printer.PushAttribute("w1", mat[0][3]);

    printer.PushAttribute("x2", mat[1][0]);
    printer.PushAttribute("y2", mat[1][1]);
    printer.PushAttribute("z2", mat[1][2]);
    printer.PushAttribute("w2", mat[1][3]);

    printer.PushAttribute("x3", mat[2][0]);
    printer.PushAttribute("y3", mat[2][1]);
    printer.PushAttribute("z3", mat[2][2]);
    printer.PushAttribute("w3", mat[2][3]);

    printer.PushAttribute("x4", mat[3][0]);
    printer.PushAttribute("y4", mat[3][1]);
    printer.PushAttribute("z4", mat[3][2]);
    printer.PushAttribute("w4", mat[3][3]);

}
void SceneSerializer::Helpers::SerializeOrthoCameraComponent(tinyxml2::XMLPrinter& printer, const OrthoCameraComponent& camera) {
    // TODO implement
}
void SceneSerializer::Helpers::SerializePerspectiveCameraComponent(tinyxml2::XMLPrinter& printer, const PerspectiveCameraComponent& camera) {
    // TODO implement
}    /*
void SceneSerializer::Helpers::SerializeScriptComponent(tinyxml2::XMLPrinter& printer, const ScriptComponent& script) {

    printer.PushAttribute("name", script._name.c_str());
    auto* type = script._underlyingInstance->GetObjectType();

    const std::vector<PropertyData>& props = script.Properties();
    for (int i = 0; i < props.size(); i++) {
        const PropertyData& prop = props[i];
        if (prop.isPrivate || prop.isProtected) { continue; } // don't serialize private or protected variables as they are never visible in editor!
        printer.OpenElement(prop.name.c_str());
        {
            auto it = Core::GetCore()->Angel()->_enums.find(prop.typeName);
            if (it != Core::GetCore()->Angel()->_enums.end()) SerializeEnum(printer, prop.typeName.c_str(), script.GetAt<int>(i));
            else if (prop.typeName == "Entity")        SerializeEntityID(printer, script.GetAt<Entity>(i));
            else if (prop.typeName == "int")           SerializeInt(printer, script.GetAt<int>(i));
            else if (prop.typeName == "float")         SerializeFloat(printer, script.GetAt<float>(i));
            else if (prop.typeName == "double")        SerializeDouble(printer, script.GetAt<double>(i));
            else if (prop.typeName == "string")        SerializeString(printer, script.GetAt<std::string>(i).c_str());
            else if (prop.typeName == "vec2")          SerializeVec2(printer, script.GetAt<glm::vec2>(i));
            else if (prop.typeName == "vec3")          SerializeVec3(printer, script.GetAt<glm::vec3>(i));
            else if (prop.typeName == "vec4")          SerializeVec4(printer, script.GetAt<glm::vec4>(i));
            else if (prop.typeName == "quat")          SerializeQuat(printer, script.GetAt<glm::quat>(i));
            else if (prop.typeName == "mat2")          SerializeMat2(printer, script.GetAt<glm::mat2>(i));
            else if (prop.typeName == "mat3")          SerializeMat3(printer, script.GetAt<glm::mat3>(i));
            else if (prop.typeName == "mat4")          SerializeMat4(printer, script.GetAt<glm::mat4>(i));
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
        printer.CloseElement(); // prop.name.c_str() end
    }
}*/