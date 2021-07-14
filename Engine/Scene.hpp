#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <entt.hpp>

#include "Renderer.hpp"
#include "OutlineRenderer.hpp"
#include "OrthoCamera.hpp"
#include "PerspectiveCamera.hpp"

#include "TypedefsAndConstants.hpp"

struct Angel;
struct ScriptComponent;

struct Scene {
	std::string _name;
	glm::vec3 ClearColor{ 0.2f, 0.3f, 0.3f };
	glm::vec3 SelectionOutlineColor{ 0.68f, 0.49f, 0 };

	std::vector<EntityID> _entities;
	Renderer _renderer;
	OutlineRenderer _outlineRenderer;

	OrthoCamera _oc{ OrthoCamera(-1, 1, -1, 1, -1, 1) };
	PerspectiveCamera _pc{ PerspectiveCamera(110, 19.f/9.f, 0.001, 1000) };
	Camera* _activeCamera; // address of active cam

	entt::registry _registry;
	std::unordered_map<int, std::vector<std::string>> _attachList; // scripts that call Attach add to this map
	std::unordered_map<int, std::vector<std::string>> _detachList; // scripts that call Detach add to this map

	EntityID CreateEntity(const char* name = "Entity", int desiredID = -1);
	void DeleteEntity(EntityID id);
	bool ContainsEntity(EntityID id);

	ScriptComponent& Modules(EntityID id);

	void SetOrthoCamera(float left, float right, float bottom, float top, float near, float far);
	void SetPerpectiveCamera(float fov, float aspect, float near, float far);

	void Update(const std::unique_ptr<Angel>& angel, float delta);
	void Render(const std::unique_ptr<Angel>& angel);

	//don't call, use CreateScene
	Scene(std::string_view name) noexcept;
	~Scene() noexcept;
	Scene(const Scene&) = delete;
	Scene(const Scene&&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene& operator=(const Scene&&) = delete;
};

std::weak_ptr<Scene> CreateScene(std::string_view name = "New Scene");
void ChangeScene(std::weak_ptr<Scene> scene);
std::weak_ptr<Scene> FindSceneByName(std::string_view name);
std::weak_ptr<Scene> FindActiveScene();
void DeleteAllScenes();