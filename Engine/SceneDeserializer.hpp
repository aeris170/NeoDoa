#pragma once

#include <string>
#include <string_view>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <tinyxml2.h>

#include "Entity.hpp"

struct Scene;
struct FNode;
struct IDComponent;
struct TransformComponent;
struct ParentComponent;
struct ChildComponent;
struct OrthoCameraComponent;
struct PerspectiveCameraComponent;

Scene DeserializeScene(const FNode& file);
Scene DeserializeScene(const std::string& data);

Entity DeserializeEntityID(const tinyxml2::XMLElement* property, std::string_view identifierOverride = "id");
int DeserializeEnum(const tinyxml2::XMLElement* property);
int DeserializeInt(const tinyxml2::XMLElement* property);
float DeserializeFloat(const tinyxml2::XMLElement* property);
double DeserializeDouble(const tinyxml2::XMLElement* property);
std::string DeserializeString(const tinyxml2::XMLElement* property);
glm::vec2 DeserializeVec2(const tinyxml2::XMLElement* property);
glm::vec3 DeserializeVec3(const tinyxml2::XMLElement* property);
glm::vec4 DeserializeVec4(const tinyxml2::XMLElement* property);
glm::quat DeserializeQuat(const tinyxml2::XMLElement* property);
glm::mat2 DeserializeMat2(const tinyxml2::XMLElement* property);
glm::mat3 DeserializeMat3(const tinyxml2::XMLElement* property);
glm::mat4 DeserializeMat4(const tinyxml2::XMLElement* property);
IDComponent DeserializeIDComponent(const tinyxml2::XMLElement* component);
TransformComponent DeserializeTransformComponent(const tinyxml2::XMLElement* component, const Entity entity);
ParentComponent DeserializeParentComponent(const tinyxml2::XMLElement* component, const Entity entity);
ChildComponent DeserializeChildComponent(const tinyxml2::XMLElement* component, const Entity entity);
OrthoCameraComponent DeserializeOrthoCameraComponent(const tinyxml2::XMLElement* component, const Entity entity, const Scene& scene);
PerspectiveCameraComponent DeserializePerspectiveCameraComponent(const tinyxml2::XMLElement* component, const Entity entity, const Scene& scene);
//ScriptComponentData DeserializeScriptComponentData(const tinyxml2::XMLElement* component, const Entity entity);
