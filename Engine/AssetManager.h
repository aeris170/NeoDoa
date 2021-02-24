#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "Texture.hpp"
#include "Model.hpp"

struct AssetManager {

	std::string root;

	std::unordered_map<std::string, std::string> scripts;
	std::unordered_map<std::string, std::weak_ptr<Texture>> m_textures;
	std::unordered_map<std::string, std::weak_ptr<Model>> m_meshes;
	//std::unordered_map<std::string, Material*> m_materials;
};
