#pragma once

#include <angelscript.h>

#include "Module.hpp"

struct Tag : Module {

	using Module::Module; // inherit ctors

	std::string& Label() const;

};
