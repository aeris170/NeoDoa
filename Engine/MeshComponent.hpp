#pragma once

#include <memory>

#include "Entity.hpp"

struct Mesh;

namespace tinyxml2 {
	class XMLPrinter;
	class XMLElement;
}

struct MeshComponent {
private:

	Entity entity{ NULL_ENTT };
	std::weak_ptr<Mesh> mesh{};

public:
	MeshComponent(Entity entity) noexcept;
	MeshComponent(Entity entity, std::weak_ptr<Mesh> mesh) noexcept;

	Entity GetEntity() const;

	std::weak_ptr<Mesh> GetMesh() const;
	void SetMesh(std::weak_ptr<Mesh> mesh);

	friend void SerializeIDComponent(tinyxml2::XMLPrinter& printer, const MeshComponent& id);
	friend MeshComponent DeserializeIDComponent(tinyxml2::XMLElement* property);
};