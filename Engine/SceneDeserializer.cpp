#include <Engine/SceneDeserializer.hpp>

#include <Utility/NameOf.hpp>

#include <Engine/Log.hpp>
#include <Engine/Core.hpp>
#include <Engine/Angel.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Entity.hpp>
#include <Engine/FileNode.hpp>
#include <Engine/IDComponent.hpp>
#include <Engine/PropertyData.hpp>
#include <Engine/ChildComponent.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/CameraComponent.hpp>
#include <Engine/TransformComponent.hpp>

Scene DeserializeScene(const FNode& file) {
    file.ReadContent();
    return DeserializeScene(file.DisposeContent());
}
Scene DeserializeScene(const std::string& data) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.c_str());
    if (err != tinyxml2::XML_SUCCESS) {
        DOA_LOG_ERROR("Couldn't deserialize scene!\n\n%s", data);
    }

    Scene scene{};
    SceneDeserializer::Deserialize(*doc.RootElement(), scene);
    return scene;
}

void SceneDeserializer::DefaultDeserialize(tinyxml2::XMLElement& rootNode, Scene& scene) {
    SceneConfig::Deserialize(*rootNode.FirstChildElement("config"), scene);
    Entities::Deserialize(*rootNode.FirstChildElement("entities"), scene);
}

void SceneDeserializer::SceneConfig::DefaultDeserialize(tinyxml2::XMLElement& configNode, Scene& scene) {
    DeserializeName(configNode, scene);
    DeserializeClearColor(configNode, scene);
    DeserializeSelectionOutlineColor(configNode, scene);
    Cameras::Deserialize(*configNode.FirstChildElement("cameras"), scene);
}
void SceneDeserializer::SceneConfig::DefaultDeserializeName(tinyxml2::XMLElement& configNode, Scene& scene) {
    scene.Name = configNode.Attribute("name");
}
void SceneDeserializer::SceneConfig::DefaultDeserializeClearColor(tinyxml2::XMLElement& configNode, Scene& scene) {
    const tinyxml2::XMLElement& clearColor = *configNode.FirstChildElement("clearColor");
    scene.ClearColor = {
        clearColor.FloatAttribute("r"),
        clearColor.FloatAttribute("g"),
        clearColor.FloatAttribute("b")
    };
}
void SceneDeserializer::SceneConfig::DefaultDeserializeSelectionOutlineColor(tinyxml2::XMLElement& configNode, Scene& scene) {
    const tinyxml2::XMLElement& selectionOutlineColor = *configNode.FirstChildElement("selectionOutlineColor");
    scene.SelectionOutlineColor = {
        selectionOutlineColor.FloatAttribute("r"),
        selectionOutlineColor.FloatAttribute("g"),
        selectionOutlineColor.FloatAttribute("b")
    };
}

void SceneDeserializer::SceneConfig::Cameras::DefaultDeserialize(tinyxml2::XMLElement& camerasNode, Scene& scene) {
    //DeserializeActiveCamera(*camerasNode.FirstChildElement("activeCamera"), scene);
    //DeserializeOrthoCamera(*camerasNode.FirstChildElement("orthoCamera"), scene);
    //DeserializePerspectiveCamera(*camerasNode.FirstChildElement("perspectiveCamera"), scene);
}
void SceneDeserializer::SceneConfig::Cameras::DefaultDeserializeActiveCamera(tinyxml2::XMLElement& activeCameraNode, Scene& scene) {
    //std::string activeCamType = activeCameraNode.Attribute("type");

    //if (activeCamType == "ortho") {
    //    scene.SwitchToOrtho();
    //} else if (activeCamType == "perspective") {
    //    scene.SwitchToPerspective();
    //}
    //ACamera& activeCamera = scene.GetActiveCamera();

    //{
    //    const tinyxml2::XMLElement* eye = activeCameraNode.FirstChildElement("eye");
    //    activeCamera.eye = { eye->FloatAttribute("x"), eye->FloatAttribute("y"), eye->FloatAttribute("z") };
    //}
    //{
    //    const tinyxml2::XMLElement* forward = activeCameraNode.FirstChildElement("forward");
    //    activeCamera.forward = { forward->FloatAttribute("x"), forward->FloatAttribute("y"), forward->FloatAttribute("z") };
    //}
    //{
    //    const tinyxml2::XMLElement* up = activeCameraNode.FirstChildElement("up");
    //    activeCamera.up = { up->FloatAttribute("x"), up->FloatAttribute("y"), up->FloatAttribute("z") };
    //}
    //{
    //    const tinyxml2::XMLElement* zoom = activeCameraNode.FirstChildElement("zoom");
    //    activeCamera.zoom = { zoom->FloatAttribute("value") };
    //}
}
void SceneDeserializer::SceneConfig::Cameras::DefaultDeserializeOrthoCamera(tinyxml2::XMLElement& orthoCameraNode, Scene& scene) {
    /*scene.SetOrtho(orthoCameraNode.FloatAttribute("left"),   orthoCameraNode.FloatAttribute("right"),
                   orthoCameraNode.FloatAttribute("bottom"), orthoCameraNode.FloatAttribute("top"),
                   orthoCameraNode.FloatAttribute("near"),   orthoCameraNode.FloatAttribute("far"));*/
}
void SceneDeserializer::SceneConfig::Cameras::DefaultDeserializePerspectiveCamera(tinyxml2::XMLElement& perspectiveCameraNode, Scene& scene) {
    /*scene.SetPerpective(perspectiveCameraNode.FloatAttribute("fov"),  perspectiveCameraNode.FloatAttribute("aspect"),
                        perspectiveCameraNode.FloatAttribute("near"), perspectiveCameraNode.FloatAttribute("far"));*/
}

void SceneDeserializer::Entities::DefaultDeserialize(tinyxml2::XMLElement& entitiesNode, Scene& scene) {
    for (tinyxml2::XMLElement* entity = entitiesNode.FirstChildElement(); entity != nullptr; entity = entity->NextSiblingElement()) {
        DeserializeEntity(*entity, scene);
    }
}
void SceneDeserializer::Entities::DefaultDeserializeEntity(tinyxml2::XMLElement& entityNode, Scene& scene) {
    Entity entt = NULL_ENTT; // the entt we are deserializing for, will be read from IDComponent Deserializer!
    for (tinyxml2::XMLElement* component = entityNode.FirstChildElement(); component != nullptr; component = component->NextSiblingElement()) {
        std::string type = component->Name();
        std::string name = component->Attribute("name");
        if (type == "cpp-component") {
            if (name == nameof(IDComponent)) {
                DeserializeIDComponent(*component, scene, entt);
            } else if (name == nameof(TransformComponent)) {
                DeserializeTransformComponent(*component, scene, entt);
            } else if (name == nameof(ParentComponent)) {
                DeserializeParentComponent(*component, scene, entt);
            } else if (name == nameof(ChildComponent)) {
                DeserializeChildComponent(*component, scene, entt);
            }
        } else {
            DeserializeUserDefinedComponents(*component, scene, entt, name);
        }
    }
}
void SceneDeserializer::Entities::DefaultDeserializeIDComponent(tinyxml2::XMLElement& componentNode, Scene& scene, Entity& readEntity) {
    auto entity = Helpers::DeserializeEntityID(*componentNode.FirstChildElement(nameof_c(IDComponent::entity)));
    auto tag = componentNode.FirstChildElement(nameof_c(IDComponent::tag))->Attribute("value");
    IDComponent id{ entity, tag };
    readEntity = scene.CreateEntity(id.GetTagRef(), static_cast<uint32_t>(id.GetEntity()));
    scene.RemoveComponent<TransformComponent>(id.GetEntity()); /* remove tr-cmp bevause we will deserialize one from file */
}
void SceneDeserializer::Entities::DefaultDeserializeTransformComponent(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity) {
    auto t = Helpers::DeserializeVec3(*componentNode.FirstChildElement(nameof_c(TransformComponent::localTranslation)));
    auto r = Helpers::DeserializeQuat(*componentNode.FirstChildElement(nameof_c(TransformComponent::localRotation)));
    auto s = Helpers::DeserializeVec3(*componentNode.FirstChildElement(nameof_c(TransformComponent::localScale)));
    TransformComponent transform{ entity };
    transform.SetLocalTranslation(t);
    transform.SetLocalRotation(r);
    transform.SetLocalScale(s);
    scene.InsertComponent<TransformComponent>(transform.GetEntity(), std::move(transform));
}
void SceneDeserializer::Entities::DefaultDeserializeParentComponent(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity) {
    std::vector<Entity> children;
    for (
        tinyxml2::XMLElement* childNode = componentNode.FirstChildElement(nameof_c(ParentComponent::children))->FirstChildElement()->FirstChildElement();
        childNode != nullptr;
        childNode = childNode->NextSiblingElement()) {
        auto child = Helpers::DeserializeEntityID(*childNode, "value");
        children.push_back(child);
    }
    ParentComponent parent{ entity, std::move(children) };
    scene.InsertComponent<ParentComponent>(parent.GetEntity(), std::move(parent));
}
void SceneDeserializer::Entities::DefaultDeserializeChildComponent(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity) {
    auto parent = Helpers::DeserializeEntityID(*componentNode.FirstChildElement(nameof_c(ChildComponent::parent)));
    ChildComponent children{ entity, parent };
    scene.InsertComponent<ChildComponent>(children.GetEntity(), std::move(children));
}
void SceneDeserializer::Entities::DefaultDeserializeUserDefinedComponents(tinyxml2::XMLElement&, Scene&, Entity, const std::string& name) {}


/*
 int DeserializeEnum(const tinyxml2::XMLElement& property);
 Entity DeserializeEntityID(const tinyxml2::XMLElement& property, std::string_view identifierOverride = "id");
 int8_t DeserializeInt8  (const tinyxml2::XMLElement& property);
 int16_t DeserializeInt16 (const tinyxml2::XMLElement& property);
 int32_t DeserializeInt32 (const tinyxml2::XMLElement& property);
 int64_t DeserializeInt64 (const tinyxml2::XMLElement& property);
 uint8_t DeserializeUInt8 (const tinyxml2::XMLElement& property);
 uint16_t DeserializeUInt16(const tinyxml2::XMLElement& property);
 uint32_t DeserializeUInt32(const tinyxml2::XMLElement& property);
 uint64_t DeserializeUInt64(const tinyxml2::XMLElement& property);
 int DeserializeInt(const tinyxml2::XMLElement& property);
 float DeserializeFloat(const tinyxml2::XMLElement& property);
 double DeserializeDouble(const tinyxml2::XMLElement& property);
 bool DeserializeBool(const tinyxml2::XMLElement& property);
*/

int SceneDeserializer::Helpers::DeserializeEnum(const tinyxml2::XMLElement& property) {
    return property.IntAttribute("value");
}
Entity SceneDeserializer::Helpers::DeserializeEntityID(const tinyxml2::XMLElement& property, std::string_view identifierOverride) {
    return static_cast<Entity>(property.UnsignedAttribute(identifierOverride.data(), -1));
}
int8_t SceneDeserializer::Helpers::DeserializeInt8(const tinyxml2::XMLElement& property) {
    return static_cast<int8_t>(property.IntAttribute("value"));
}
int16_t SceneDeserializer::Helpers::DeserializeInt16(const tinyxml2::XMLElement& property) {
    return static_cast<int16_t>(property.IntAttribute("value"));
}
int32_t SceneDeserializer::Helpers::DeserializeInt32(const tinyxml2::XMLElement& property) {
    return property.IntAttribute("value");
    }
int64_t SceneDeserializer::Helpers::DeserializeInt64(const tinyxml2::XMLElement& property) {
    return property.Int64Attribute("value");
}
uint8_t SceneDeserializer::Helpers::DeserializeUInt8(const tinyxml2::XMLElement& property) {
    return static_cast<uint8_t>(property.UnsignedAttribute("value"));
}
uint16_t SceneDeserializer::Helpers::DeserializeUInt16(const tinyxml2::XMLElement& property) {
    return static_cast<uint16_t>(property.UnsignedAttribute("value"));
}
uint32_t SceneDeserializer::Helpers::DeserializeUInt32(const tinyxml2::XMLElement& property) {
    return property.UnsignedAttribute("value");
}
uint64_t SceneDeserializer::Helpers::DeserializeUInt64(const tinyxml2::XMLElement& property) {
    return property.Unsigned64Attribute("value");
}
int SceneDeserializer::Helpers::DeserializeInt(const tinyxml2::XMLElement& property) {
    return property.IntAttribute("value");
}
long SceneDeserializer::Helpers::DeserializeLong(const tinyxml2::XMLElement& property) {
    return static_cast<long>(property.Int64Attribute("value"));
}
float SceneDeserializer::Helpers::DeserializeFloat(const tinyxml2::XMLElement& property) {
    return property.FloatAttribute("value");
}
double SceneDeserializer::Helpers::DeserializeDouble(const tinyxml2::XMLElement& property) {
    return property.DoubleAttribute("value");
}
bool SceneDeserializer::Helpers::DeserializeBool(const tinyxml2::XMLElement& property) {
    return property.BoolAttribute("value");
}
std::string SceneDeserializer::Helpers::DeserializeString(const tinyxml2::XMLElement& property) {
    return property.Attribute("value");
}
glm::vec2 SceneDeserializer::Helpers::DeserializeVec2(const tinyxml2::XMLElement& property) {
    return {
        property.FloatAttribute("x", 0),
        property.FloatAttribute("y", 0)
    };
}
glm::vec3 SceneDeserializer::Helpers::DeserializeVec3(const tinyxml2::XMLElement& property) {
    return {
        property.FloatAttribute("x", 0),
        property.FloatAttribute("y", 0),
        property.FloatAttribute("z", 0)
    };
}
glm::vec4 SceneDeserializer::Helpers::DeserializeVec4(const tinyxml2::XMLElement& property) {
    return {
        property.FloatAttribute("x", 0),
        property.FloatAttribute("y", 0),
        property.FloatAttribute("z", 0),
        property.FloatAttribute("w", 0)
    };
}
glm::quat SceneDeserializer::Helpers::DeserializeQuat(const tinyxml2::XMLElement& property) {
    return {
        property.FloatAttribute("w", 0),
        property.FloatAttribute("x", 0),
        property.FloatAttribute("y", 0),
        property.FloatAttribute("z", 0)
    };
}
glm::mat2 SceneDeserializer::Helpers::DeserializeMat2(const tinyxml2::XMLElement& property) {
    return {
        property.FloatAttribute("x1", 1), property.FloatAttribute("y1", 0),
        property.FloatAttribute("x2", 0), property.FloatAttribute("y2", 1)
    };
}
glm::mat3 SceneDeserializer::Helpers::DeserializeMat3(const tinyxml2::XMLElement& property) {
    return {
        property.FloatAttribute("x1", 1), property.FloatAttribute("y1", 0), property.FloatAttribute("z1", 0),
        property.FloatAttribute("x2", 0), property.FloatAttribute("y2", 1), property.FloatAttribute("z2", 0),
        property.FloatAttribute("x3", 0), property.FloatAttribute("y3", 0), property.FloatAttribute("z3", 1)
    };
}
glm::mat4 SceneDeserializer::Helpers::DeserializeMat4(const tinyxml2::XMLElement& property) {
    return {
        property.FloatAttribute("x1", 1), property.FloatAttribute("y1", 0), property.FloatAttribute("z1", 0), property.FloatAttribute("w1", 0),
        property.FloatAttribute("x2", 0), property.FloatAttribute("y2", 1), property.FloatAttribute("z2", 0), property.FloatAttribute("w2", 0),
        property.FloatAttribute("x3", 0), property.FloatAttribute("y3", 0), property.FloatAttribute("z3", 1), property.FloatAttribute("w3", 0),
        property.FloatAttribute("x4", 0), property.FloatAttribute("y4", 0), property.FloatAttribute("z4", 0), property.FloatAttribute("w4", 1)
    };
}
/*
ScriptComponentData DeserializeScriptComponentData(const tinyxml2::XMLElement* component, const Entity entity) {


    ScriptComponentData data;
    data.name = component->Attribute("name");

    loopElement(component->FirstChildElement(), [&data](tinyxml2::XMLElement* l) {
        std::any value;

        std::string type = l->Attribute("type");
        auto it = Core::GetCore()->Angel()->_enums.find(type);
        if (it != Core::GetCore()->Angel()->_enums.end())	value = DeserializeEnum(l);
        else if (type == "Entity")					value = DeserializeEntityID(l);
        else if (type == "int")						value = DeserializeInt(l);
        else if (type == "float")					value = DeserializeFloat(l);
        else if (type == "double")					value = DeserializeDouble(l);
        else if (type == "string")					value = DeserializeString(l);
        else if (type == "vec2")					value = DeserializeVec2(l);
        else if (type == "vec3")					value = DeserializeVec3(l);
        else if (type == "vec4")					value = DeserializeVec4(l);
        else if (type == "quat")					value = DeserializeQuat(l);
        else if (type == "mat2")					value = DeserializeMat2(l);
        else if (type == "mat3")					value = DeserializeMat3(l);
        else if (type == "mat4")					value = DeserializeMat4(l);
        else {
            //if (prop.isReference) {
                // TODO
            DOA_LOG_TRACE("prop.isReference ??");
            //}
            //else {
                // TODO
            DOA_LOG_TRACE("!prop.isReference ??");
            //}
        }

        data.data.emplace_back(value);
    });
    return data;
}
*/
