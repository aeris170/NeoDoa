#include "Scene.hpp"

#include "Shader.hpp"
#include "Model.hpp"
#include "Renderer.hpp"

#include "Angel.hpp"
#include "ScriptComponent.hpp"

#include "Tag.hpp"
#include "Transform.hpp"

static std::shared_ptr<Scene> ACTIVE_SCENE;
static std::unordered_map<std::string, std::shared_ptr<Scene>> SCENES;

Scene::Scene(const char* name) noexcept :
	_name(name),
	_activeCamera(&_oc) {} // default camera is orthographic

Scene::~Scene() {}

EntityID Scene::CreateEntity(const char* name) {
	auto id = _registry.create();
	ScriptComponent& scr = _registry.emplace<ScriptComponent>(id, static_cast<int>(id));

	// attach default modules
	Tag& tag = scr.Attach("Tag").As<Tag>();
	tag.Label() = name;
	scr.Attach("Transform");

	return id;
}

void Scene::DeleteEntity(EntityID id) {
	_registry.destroy(id);
}

bool Scene::ContainsEntity(EntityID id) {
	return _registry.valid(id);
}

ScriptComponent& Scene::Modules(EntityID id) {
	return _registry.get<ScriptComponent>(id);
}

void Scene::SetOrthoCamera(float left, float right, float bottom, float top, float near, float far) {
	_oc = OrthoCamera(left, right, bottom, top, near, far);
	_activeCamera = &_oc;
}

void Scene::SetPerpectiveCamera(float fov, float aspect, float near, float far) {
	_pc = PerspectiveCamera(fov, aspect, near, far);
	_activeCamera = &_pc;
}

void Scene::Update(const std::unique_ptr<Angel>& angel, float deltaTime) {
	for (auto& [id, list] : _attachList) {
		auto& modules = Modules(static_cast<EntityID>(id));
		for (auto& type : list) {
			modules.Attach(type, false);
		}
	}
	_attachList.clear();
	for (auto& [id, list] : _detachList) {
		auto& modules = Modules(static_cast<EntityID>(id));
		for (auto& type : list) {
			modules.Detach(type, false);
		}
	}
	_detachList.clear();
	// this will need optimization... anti-pattern to ECS!
	_registry.view<ScriptComponent>().each([this, &angel, deltaTime](EntityID entity, ScriptComponent& script) {
		for (auto& [name, module] : script._modules) {
			angel->ExecuteModule(module, deltaTime);
		}
	});
}

void Scene::Render(const std::unique_ptr<Angel>& angel) {
	_activeCamera->UpdateView();
	_activeCamera->UpdateProjection();
	_activeCamera->UpdateViewProjection();
	_registry.view<ScriptComponent>().each([this, &angel](EntityID entity, ScriptComponent& script) {
		for (auto& [name, module] : script._modules) {
			if (name == "ModelRenderer") {
				_renderer.Register((*(Shader**)module->GetAddressOfProperty(2))->shared_from_this(), (*(Model**)module->GetAddressOfProperty(1))->shared_from_this());
				_renderer.Submit((*(Shader**)module->GetAddressOfProperty(2))->shared_from_this(), (*(Model**)module->GetAddressOfProperty(1))->shared_from_this(), entity);
			}
		}
	});
	_renderer.Render(_registry, _activeCamera);
}

//-----------------------------------------------------------------

std::weak_ptr<Scene> CreateScene(const char* name) {
	auto rv = std::make_shared<Scene>(name);
	SCENES.insert({ name, rv });
	return rv;
}

void ChangeScene(std::weak_ptr<Scene> scene) {
	ACTIVE_SCENE = scene.lock();
}

std::weak_ptr<Scene> FindSceneByName(const char* name) {
	return SCENES.find(name)->second;
}

std::weak_ptr<Scene> FindActiveScene() {
	return ACTIVE_SCENE;
}
