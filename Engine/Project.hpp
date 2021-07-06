#pragma once

#include <string>

#include "AssetManager.hpp"
#include "Scene.hpp"

struct Project {
	std::string _name;
	AssetManager _assets;
	std::weak_ptr<Scene> _startupScene;

	Project(std::string path) noexcept;
	Project(std::string path, std::string name) noexcept;

	void Serialize(std::string path);
	void Close();
};