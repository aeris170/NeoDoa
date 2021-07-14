#include "Scene.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model.hpp"
#include "ModelRenderer.hpp"
#include "Renderer.hpp"

#include "Angel.hpp"
#include "ScriptComponent.hpp"

#include "Tag.hpp"
#include "Transform.hpp"
#include "Log.hpp"
#include "Texture.hpp"

static std::shared_ptr<Scene> ACTIVE_SCENE;
static std::unordered_map<std::string, std::shared_ptr<Scene>> SCENES;

Scene::Scene(std::string_view name) noexcept :
	_name(name),
	_activeCamera(&_oc) {} // default camera is orthographic

Scene::~Scene() {}

EntityID Scene::CreateEntity(const char* name, int desiredID) {
	EntityID id;
	if (desiredID == -1) {
		id = _registry.create();
	} else {
		id = _registry.create(static_cast<EntityID>(desiredID));
	}
	ScriptComponent& scr = _registry.emplace<ScriptComponent>(id, static_cast<int>(id));

	// attach default modules
	Tag& tag = scr.Attach("Tag").As<Tag>();
	tag._isDef = true;
	tag.Label() = name;
	Transform& transform = scr.Attach("Transform").As<Transform>();
	transform._isDef = true;

	_entities.push_back(id);
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
		for (auto& [name, module, isDef] : script._modules) {
			angel->ExecuteModule(module, deltaTime);
		}
	});
}

void Scene::Render(const std::unique_ptr<Angel>& angel) {
	_activeCamera->UpdateView();
	_activeCamera->UpdateProjection();
	_activeCamera->UpdateViewProjection();
	_registry.view<ScriptComponent>().each([this, &angel](EntityID entity, ScriptComponent& script) {
		for (auto& module : script._modules) {
			if (module._name == "ModelRenderer") {
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

#ifdef EDITOR
	// Find selected objects
	std::vector<std::tuple<Transform&, ModelRenderer&>> selecteds;
	_registry.view<ScriptComponent>().each([&](EntityID entity, ScriptComponent& script) {
		Transform& t = script["Transform"].As<Transform>();
		if (t.Selected()) {
			ModelRenderer& mr = script["ModelRenderer"].As<ModelRenderer>();
			selecteds.push_back({ t, mr });
		}
	});

	// Outline them
	_outlineRenderer.Render(selecteds, _activeCamera, SelectionOutlineColor);
#endif
}

//-----------------------------------------------------------------

std::weak_ptr<Scene> CreateScene(std::string_view name) {
	auto rv = std::make_shared<Scene>(name);
	SCENES.insert({ std::string(name), rv });
	return rv;
}

void ChangeScene(std::weak_ptr<Scene> scene) {
	ACTIVE_SCENE = scene.lock();
	if (scene.expired()) {
		DOA_LOG_INFO("Changed active scene to NONE.");
	} else {
		DOA_LOG_INFO("Changed active scene to %s", ACTIVE_SCENE->_name.c_str());
	}
}

std::weak_ptr<Scene> FindSceneByName(std::string_view name) {
	auto itr = SCENES.find(std::string(name));
	if (itr != SCENES.end()) {
		return itr->second;
	}
	return std::weak_ptr<Scene>();
}

std::weak_ptr<Scene> FindActiveScene() {
	return ACTIVE_SCENE;
}

void DeleteAllScenes() {
	ACTIVE_SCENE = nullptr;
	SCENES.clear();
}
