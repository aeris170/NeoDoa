#pragma once

#include <tinyxml2.h>

#include <Scene.hpp>
#include <Entity.hpp>
#include <SceneSerializer.hpp>

void SerializeUserDefinedComponentStorage(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity);