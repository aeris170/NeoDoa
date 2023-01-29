#pragma once

#include <string>

#include <tinyxml2.h>

#include <Scene.hpp>
#include <Entity.hpp>

void DeserializeUserDefinedComponentStorage(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity, const std::string& name);