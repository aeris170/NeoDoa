#pragma once

#include <string>
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <tinyxml2.h>

#include "Entity.hpp"

struct Scene;
struct IDComponent;
struct TransformComponent;
struct ParentComponent;
struct ChildComponent;
struct ScriptStorageComponent;
struct ScriptComponent;

std::string SerializeScene(const Scene& scene);

void SerializeEntityID(tinyxml2::XMLPrinter& printer, Entity entity);
void SerializeEnum(tinyxml2::XMLPrinter& printer, std::string_view name, int value);
void SerializeInt(tinyxml2::XMLPrinter& printer, int value);
void SerializeFloat(tinyxml2::XMLPrinter& printer, float value);
void SerializeDouble(tinyxml2::XMLPrinter& printer, double value);
void SerializeString(tinyxml2::XMLPrinter& printer, std::string_view value);
void SerializeVec2(tinyxml2::XMLPrinter& printer, const glm::vec2& vec);
void SerializeVec3(tinyxml2::XMLPrinter& printer, const glm::vec3& vec);
void SerializeVec4(tinyxml2::XMLPrinter& printer, const glm::vec4& vec);
void SerializeQuat(tinyxml2::XMLPrinter& printer, const glm::quat& quat);
void SerializeMat2(tinyxml2::XMLPrinter& printer, const glm::mat2& mat);
void SerializeMat3(tinyxml2::XMLPrinter& printer, const glm::mat3& mat);
void SerializeMat4(tinyxml2::XMLPrinter& printer, const glm::mat4& mat);
void SerializeIDComponent(tinyxml2::XMLPrinter& printer, const IDComponent& id);
void SerializeTransformComponent(tinyxml2::XMLPrinter& printer, const TransformComponent& transform);
void SerializeParentComponent(tinyxml2::XMLPrinter& printer, const ParentComponent& parent);
void SerializeChildComponent(tinyxml2::XMLPrinter& printer, const ChildComponent& child);
void SerializeScriptComponent(tinyxml2::XMLPrinter& printer, const ScriptComponent& script);
