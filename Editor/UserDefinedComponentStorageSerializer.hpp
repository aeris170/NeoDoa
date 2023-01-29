#pragma once

#include <tinyxml2.h>

#include <Scene.hpp>
#include <Entity.hpp>

void SerializeUserDefinedComponentStorage(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity);