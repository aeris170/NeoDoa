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
struct ScriptComponentData;

Scene DeserializeScene(FNode* file);

Entity DeserializeEntityID(tinyxml2::XMLElement* property, std::string_view identifierOverride = "id");
int DeserializeEnum(tinyxml2::XMLElement* property);
int DeserializeInt(tinyxml2::XMLElement* property);
float DeserializeFloat(tinyxml2::XMLElement* property);
double DeserializeDouble(tinyxml2::XMLElement* property);
std::string DeserializeString(tinyxml2::XMLElement* property);
glm::vec2 DeserializeVec2(tinyxml2::XMLElement* property);
glm::vec3 DeserializeVec3(tinyxml2::XMLElement* property);
glm::vec4 DeserializeVec4(tinyxml2::XMLElement* property);
glm::quat DeserializeQuat(tinyxml2::XMLElement* property);
glm::mat2 DeserializeMat2(tinyxml2::XMLElement* property);
glm::mat3 DeserializeMat3(tinyxml2::XMLElement* property);
glm::mat4 DeserializeMat4(tinyxml2::XMLElement* property);
IDComponent DeserializeIDComponent(tinyxml2::XMLElement* component);
TransformComponent DeserializeTransformComponent(tinyxml2::XMLElement* component, const Entity entity, const Scene& scene);
ParentComponent DeserializeParentComponent(tinyxml2::XMLElement* component, const Entity entity);
ChildComponent DeserializeChildComponent(tinyxml2::XMLElement* component, const Entity entity);
ScriptComponentData DeserializeScriptComponentData(tinyxml2::XMLElement* component, const Entity entity);