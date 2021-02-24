#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "Mesh.hpp"

struct Model : std::enable_shared_from_this<Model> {
	std::string _name;
	std::vector<Mesh> _meshes;

	Model(std::string_view name, std::vector<Mesh>&& meshes) noexcept;
	~Model() noexcept;
};

std::weak_ptr<Model> CreateModelFromMesh(std::string_view name, std::vector<Mesh>& meshes);
std::optional<std::weak_ptr<Model>> CreateModelFromFileSystem(std::string_view modelPath);
std::optional<std::weak_ptr<Model>> FindModel(std::string_view name);