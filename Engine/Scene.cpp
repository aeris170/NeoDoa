#include "Scene.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model.hpp"
#include "ModelRenderer.hpp"
#include "Renderer.hpp"

#include "Core.hpp"

#include "Log.hpp"
#include "Texture.hpp"
#include "TransformComponent.hpp"
#include "IDComponent.hpp"
#include "ParentComponent.hpp"
#include "CameraComponent.hpp"
#include "Project.hpp"
#include "SceneSerializer.hpp"
#include "SceneDeserializer.hpp"

Scene& Scene::GetLoadedScene() {
    static auto& core = Core::GetCore();
    assert(core->GetLoadedProject() != nullptr); // There is no loaded project, hence no loaded scene.
    assert(core->GetLoadedProject()->HasOpenScene()); // There is no loaded scene.
    return core->GetLoadedProject()->GetOpenScene();
}

Scene::Scene(std::string_view name) noexcept :
    Name(name) {}
Scene::Scene(std::string&& name) noexcept :
    Name(std::move(name)) {}

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

void Scene::SetOrtho(float left, float right, float bottom, float top, float near, float far) { _oc.Set(left, right, bottom, top, near, far); }
void Scene::SetPerpective(float fov, float aspect, float near, float far) { _pc.Set(fov, aspect, near, far); }

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
    }
    assert(false); // no camera?
    throw 1;
}

Renderer::Stats Scene::GetRendererStats() const { return _renderer.stats; }

Entity Scene::CreateEntity(std::string name, uint32_t desiredID) {
    Entity entt;
    if (desiredID != EntityTo<uint32_t>(NULL_ENTT)) {
        entt = _registry.create(Entity(desiredID));
    } else {
        entt = _registry.create();
    }

    if (name.empty()) {
        name = "New Entity ";
        name.append(std::to_string(entt));
    }
    _registry.emplace<IDComponent>(entt, entt, name);
    _registry.emplace<TransformComponent>(entt, entt);

    _entities.push_back(entt);
    return entt;
}

void Scene::DeleteEntity(Entity entt) {
    if (HasComponent<ParentComponent>(entt)) {
        auto& parent = GetComponent<ParentComponent>(entt);
        for (const auto& child : parent.GetChildren()) {
            DeleteEntity(child);
        }
    }

    _registry.destroy(entt);
    std::erase(_entities, entt);
}

bool Scene::ContainsEntity(Entity entt) const { return _registry.valid(entt); }
size_t Scene::EntityCount() const { return _registry.alive(); }
const std::vector<Entity>& Scene::GetAllEntites() const { return _entities; }

Registry& Scene::GetRegistry() { return _registry; }

std::string Scene::Serialize() const { return SerializeScene(*this); }
Scene Scene::Deserialize(const std::string& data) { return DeserializeScene(data); }

Scene Scene::Copy(const Scene& scene) { return scene.Deserialize(scene.Serialize()); }

void Scene::Update(float deltaTime) {
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

    _registry.view<OrthoCameraComponent>().each([](Entity entt, OrthoCameraComponent& camera) {
        if (!camera.IsActiveAndRendering()) { return; }
        camera.UpdateMatrices();
        const auto& fbo = camera.GetFrameBuffer();
        fbo.Bind();
        fbo.ClearBuffers();
        DOA_LOG_INFO("%s %d", "ortho camera attached to", EntityTo<int>(camera.GetEntity()));
        //render stuff here
        fbo.UnBind();
    });

    _registry.view<PerspectiveCameraComponent>().each([](Entity entt, PerspectiveCameraComponent& camera) {
        if (!camera.IsActiveAndRendering()) { return; }
        camera.UpdateMatrices();
        const auto& fbo = camera.GetFrameBuffer();
        fbo.Bind();
        fbo.ClearBuffers();
        DOA_LOG_INFO("%s %d", "perspective camera attached to", EntityTo<int>(camera.GetEntity()));
        //render stuff here
        fbo.UnBind();
    });

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