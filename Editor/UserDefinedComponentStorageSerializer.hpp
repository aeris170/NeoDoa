#pragma once

#include <tinyxml2.h>

#include <Engine/Scene.hpp>
#include <Engine/Entity.hpp>
#include <Engine/SceneSerializer.hpp>

void SerializeUserDefinedComponentStorage(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity);