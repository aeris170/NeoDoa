#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "Renderer.hpp"
#include "OutlineRenderer.hpp"
#include "OrthoCamera.hpp"
#include "PerspectiveCamera.hpp"
#include "System.hpp"
#include "Entity.hpp"
#include "Registry.hpp"

struct ScriptStorageComponent;

struct Scene {

	std::string _name;
	glm::vec3 ClearColor{ 0.2f, 0.3f, 0.3f };
	glm::vec3 SelectionOutlineColor{ 0.68f, 0.49f, 0 };

	std::vector<Entity> _entities;
	Renderer _renderer;
	OutlineRenderer _outlineRenderer;

	OrthoCamera _oc{ OrthoCamera(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f) };
	PerspectiveCamera _pc{ PerspectiveCamera(110.0f, 19.0f/9.0f, 0.001f, 1000.0f) };
	ACamera* _activeCamera; // address of active cam

	Registry _registry;
	std::unordered_map<Entity, std::vector<std::string>> _attachList; // scripts that call Attach add to this map
	std::unordered_map<Entity, std::vector<std::string>> _detachList; // scripts that call Detach add to this map

	std::vector<entt::poly<System>> _systems;

	Entity CreateEntity(std::string name = "New Entity", uint32_t desiredID = EntityTo<uint32_t>(NULL_ENTT));
	void DeleteEntity(Entity entt);
	bool ContainsEntity(Entity entt);
	size_t EntityCount();

	template <typename Component, typename... Args>
	void EmplaceComponent(Entity entity, Args&&... args) {
		_registry.emplace<Component>(entity, entity, std::forward<Args>(args)...);
	}

	template <typename Component>
	void InsertComponent(Entity entity, Component&& component) {
		_registry.emplace<Component>(entity, std::forward<Component>(component));
	}

	template <typename Component>
	void ReplaceComponent(Entity entity, Component&& component) {
		_registry.replace<Component>(entity, std::forward<Component>(component));
	}

	template <typename Component>
	void InsertOrReplaceComponent(Entity entity, Component&& component) {
		if (HasComponent<Component>(entity)) {
			ReplaceComponent<Component>(entity, std::forward<Component>(component));
		} else {
			EmplaceComponent<Component>(entity, std::forward<Component>(component));
		}
	}

	template <typename Component>
	void RemoveComponent(Entity entity) {
		_registry.remove<Component>(entity);
	}

	template <typename Component>
	void RemoveComponentIfExists(Entity entity) {
		if (HasComponent<Component>(entity)) {
			RemoveComponent<Component>(entity);
		}
	}

	template <typename Component>
	bool HasComponent(Entity entity) const {
		return _registry.all_of<Component>(entity);
	}

	template <typename Component>
	Component& GetComponent(Entity entity) {
		return _registry.get<Component>(entity);
	}

	template <typename Component>
	const Component GetComponent(Entity entity) const {
		return _registry.get<Component>(entity);
	}

	ScriptStorageComponent& Scripts(Entity entt);

	void SetOrthoCamera(float left, float right, float bottom, float top, float near, float far);
	void SetPerpectiveCamera(float fov, float aspect, float near, float far);

	void Update(float delta);
	void Render();

	Scene(std::string_view name = "New Scene") noexcept;

	operator std::weak_ptr<Scene>() const { return _this; }
	std::weak_ptr<Scene> _this;
};