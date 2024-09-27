#include <Engine/SceneSerializer.hpp>

#include <Utility/NameOf.hpp>

#include <Engine/Scene.hpp>
#include <Engine/IDComponent.hpp>
#include <Engine/TransformComponent.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/ChildComponent.hpp>
#include <Engine/CameraComponent.hpp>

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
        if (scene.HasComponent<OrthoCameraComponent>(entity)) {
            SerializeOrthoCameraComponent(printer, scene.GetComponent<OrthoCameraComponent>(entity));
        }
        if (scene.HasComponent<PerspectiveCameraComponent>(entity)) {
            SerializePerspectiveCameraComponent(printer, scene.GetComponent<PerspectiveCameraComponent>(entity));
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

        auto name = nameof(TransformComponent::localTranslation);
        printer.OpenElement(name.data());
        SceneSerializer::Helpers::SerializeVec3(printer, component.GetLocalTranslation());
        printer.CloseElement();

        name = nameof(TransformComponent::localRotation);
        printer.OpenElement(name.data());
        SceneSerializer::Helpers::SerializeQuat(printer, component.GetLocalRotation());
        printer.CloseElement();

        name = nameof(TransformComponent::localScale);
        printer.OpenElement(name.data());
        SceneSerializer::Helpers::SerializeVec3(printer, component.GetLocalScale());
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
void SceneSerializer::Entities::DefaultSerializeOrthoCameraComponent(tinyxml2::XMLPrinter& printer, const OrthoCameraComponent& component) {
    printer.OpenElement("cpp-component");
    {
        printer.PushAttribute("name", nameof_c(OrthoCameraComponent));

        printer.PushAttribute(nameof_c(OrthoCameraComponent::LeftPlane),   component.GetData().LeftPlane);
        printer.PushAttribute(nameof_c(OrthoCameraComponent::RightPlane),  component.GetData().RightPlane);
        printer.PushAttribute(nameof_c(OrthoCameraComponent::BottomPlane), component.GetData().BottomPlane);
        printer.PushAttribute(nameof_c(OrthoCameraComponent::TopPlane),    component.GetData().TopPlane);
        printer.PushAttribute(nameof_c(OrthoCameraComponent::NearPlane),   component.GetData().NearPlane);
        printer.PushAttribute(nameof_c(OrthoCameraComponent::FarPlane),    component.GetData().FarPlane);
    }
    printer.CloseElement();
}
void SceneSerializer::Entities::DefaultSerializePerspectiveCameraComponent(tinyxml2::XMLPrinter& printer, const PerspectiveCameraComponent& component) {
    printer.OpenElement("cpp-component");
    {
        printer.PushAttribute("name", nameof_c(PerspectiveCameraComponent));

        printer.PushAttribute(nameof_c(OrthoCameraComponent::FOV),         component.GetData().FOV);
        printer.PushAttribute(nameof_c(OrthoCameraComponent::AspectRatio), component.GetData().AspectRatio);
        printer.PushAttribute(nameof_c(OrthoCameraComponent::NearPlane),   component.GetData().NearPlane);
        printer.PushAttribute(nameof_c(OrthoCameraComponent::FarPlane),    component.GetData().FarPlane);
    }
    printer.CloseElement();
}
void SceneSerializer::Entities::DefaultSerializeUserDefinedComponents(tinyxml2::XMLPrinter&, const Scene&, Entity) {}

void SceneSerializer::Helpers::SerializeEnum(tinyxml2::XMLPrinter& printer, std::string_view name, const int value) {
    printer.PushAttribute("type", name.data());
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeEntityID(tinyxml2::XMLPrinter& printer, const Entity entity) {
    printer.PushAttribute("type", "Entity");
    printer.PushAttribute("id", static_cast<uint32_t>(entity));
}
void SceneSerializer::Helpers::SerializeInt8(tinyxml2::XMLPrinter& printer, const int8_t value) {
    printer.PushAttribute("type", "int8");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeInt16(tinyxml2::XMLPrinter& printer, const int16_t value) {
    printer.PushAttribute("type", "int16");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeInt32(tinyxml2::XMLPrinter& printer, const int32_t value) {
    printer.PushAttribute("type", "int");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeInt64(tinyxml2::XMLPrinter& printer, const int64_t value) {
    printer.PushAttribute("type", "long");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeUInt8(tinyxml2::XMLPrinter& printer, const uint8_t value) {
    printer.PushAttribute("type", "uint8");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeUInt16(tinyxml2::XMLPrinter& printer, const uint16_t value) {
    printer.PushAttribute("type", "uint16");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeUInt32(tinyxml2::XMLPrinter& printer, const uint32_t value) {
    printer.PushAttribute("type", "unsigned int");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeUInt64(tinyxml2::XMLPrinter& printer, const uint64_t value) {
    printer.PushAttribute("type", "unsigned long");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeInt(tinyxml2::XMLPrinter& printer, const int value) {
    printer.PushAttribute("type", "int");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeLong(tinyxml2::XMLPrinter& printer, const long value) {
    printer.PushAttribute("type", "int");
    printer.PushAttribute("value", static_cast<int64_t>(value));
}
void SceneSerializer::Helpers::SerializeFloat(tinyxml2::XMLPrinter& printer, const float value) {
    printer.PushAttribute("type", "float");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeDouble(tinyxml2::XMLPrinter& printer, const double value) {
    printer.PushAttribute("type", "double");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeBool(tinyxml2::XMLPrinter& printer, const bool value) {
    printer.PushAttribute("type", "bool");
    printer.PushAttribute("value", value);
}
void SceneSerializer::Helpers::SerializeString(tinyxml2::XMLPrinter& printer, const std::string_view value) {
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