#include "Scene.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model.hpp"
#include "ModelRenderer.hpp"
#include "Renderer.hpp"

#include "Angel.hpp"
#include "ScriptComponent.hpp"

#include "Log.hpp"
#include "Texture.hpp"
#include "TransformComponent.hpp"
#include "IDComponent.hpp"
#include "ScriptStorageComponent.hpp"

Scene::Scene(std::string_view name) noexcept :
	Name(name),
	_usingOrthoCamera(true),
	_usingPerspectiveCamera(false) {} // default camera is orthographic

Scene::operator std::weak_ptr<Scene>() const { return _this; }

bool Scene::IsOrtho() const { return _usingOrthoCamera; }
bool Scene::IsPerspective() const { return _usingPerspectiveCamera; }

void Scene::SwitchToOrtho() {
	_usingOrthoCamera = true;
	_usingPerspectiveCamera = false;
}
void Scene::SwitchToPerspective() {
	_usingPerspectiveCamera = true;
	_usingOrthoCamera = false;
}

void Scene::SetOrtho(float left, float right, float bottom, float top, float near, float far) { _oc = OrthoCamera(left, right, bottom, top, near, far); }
void Scene::SetPerpective(float fov, float aspect, float near, float far) { _pc = PerspectiveCamera(fov, aspect, near, far); }

void Scene::SetOrthoCamera(OrthoCamera&& ortho) { _oc = std::move(ortho); }
void Scene::SetPerspectiveCamera(PerspectiveCamera&& perspective) { _pc = std::move(perspective); }

OrthoCamera Scene::GetOrtho() const { return _oc; }
OrthoCamera& Scene::GetOrtho() { return _oc; }

PerspectiveCamera Scene::GetPerspective() const { return _pc; }
PerspectiveCamera& Scene::GetPerspective() { return _pc; }

ACamera& Scene::GetActiveCamera() {
	if (IsOrtho()) {
		return _oc;
	} else if (IsPerspective()) {
		return _pc;
	} else {
		assert("no camera?");
	}
}

Renderer::Stats Scene::GetRendererStats() const { return _renderer.stats; }

Entity Scene::CreateEntity(std::string name, uint32_t desiredID) {
	Entity entt;
	if (desiredID != EntityTo<uint32_t>(NULL_ENTT) && desiredID >= 0) {
		entt = _registry.create(Entity(desiredID));
	} else {
		entt = _registry.create();
	}

	if (name == "") {
		name = "New Entity ";
		name.append(std::to_string(entt));
	}
	_registry.emplace<IDComponent>(entt, entt, name);
	_registry.emplace<TransformComponent>(entt, entt, _this);
	_registry.emplace<ScriptStorageComponent>(entt, entt);

	_entities.push_back(entt);
	return entt;
}

void Scene::DeleteEntity(Entity entt) {
	_registry.destroy(entt);
	std::erase(_entities, entt);
}

bool Scene::ContainsEntity(Entity entt) { return _registry.valid(entt); }
size_t Scene::EntityCount() { return _registry.alive(); }
const std::vector<Entity>& Scene::GetAllEntites() const { return _entities; }

ScriptStorageComponent& Scene::Scripts(Entity entt) { return _registry.get<ScriptStorageComponent>(entt); }

Registry& Scene::GetRegistry() { return _registry; }

void Scene::Update(float deltaTime) {
	for (auto& [id, list] : _attachList) {
		auto& scripts = Scripts(id);
		for (auto& type : list) {
			scripts.Attach(type);
		}
	}
	_attachList.clear();
	for (auto& [id, list] : _detachList) {
		auto& scripts = Scripts(id);
		for (auto& type : list) {
			scripts.Detach(type);
		}
	}
	_detachList.clear();

	for (entt::poly<System>& s : _systems) {
		s->Init(_registry);
		s->Execute(_registry, deltaTime);
	}
}
void Scene::Render() {
	auto& cam = GetActiveCamera();
	cam.UpdateView();
	cam.UpdateProjection();
	cam.UpdateViewProjection();
	/*
	_registry.view<ScriptComponent>().each([this, &angel](Entity entity, ScriptComponent& script) {
		for (auto& module : script._modules) {
			if (module._name == "ModelRenderer" && module._isActive) {
				auto& modelRenderer = module.As<ModelRenderer>();
				Model*& mdl = modelRenderer.Model();
				Shader*& shdr = modelRenderer.Shader();
				_renderer.Register(
					shdr->weak_from_this(),
					mdl->weak_from_this()
				);

				_renderer.Submit(
					shdr->weak_from_this(),
					mdl->weak_from_this(),
					entity
				);
			}
		}
	});
	_renderer.Render(_registry, _activeCamera);
	*/

#ifdef EDITOR
	/*
	// Find selected objects
	std::vector<std::tuple<Transform&, ModelRenderer&>> selecteds;
	_registry.view<ScriptComponent>().each([&](Entity entity, ScriptComponent& script) {
		Transform& t = script["Transform"].As<Transform>();
		if (t.Selected()) {
			auto opt = script.TryGet("ModelRenderer");
			if(opt.has_value()) {
				ModelRenderer& mr = opt.value().get().As<ModelRenderer>();
				selecteds.push_back({ t, mr });
			}
		}
	});

	// Outline them
	_outlineRenderer.Render(selecteds, _activeCamera, SelectionOutlineColor);
	*/
#endif
}