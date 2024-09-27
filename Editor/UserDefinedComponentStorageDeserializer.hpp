#pragma once

#include <string>

#include <tinyxml2.h>

#include <Engine/Scene.hpp>
#include <Engine/Entity.hpp>
#include <Engine/SceneDeserializer.hpp>

void DeserializeUserDefinedComponentStorage(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity, [[maybe_unused]] const std::string& name);