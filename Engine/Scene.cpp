#include "Scene.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model.hpp"
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
    assert(core->LoadedProject() != nullptr); // There is no loaded project, hence no loaded scene.
    assert(core->LoadedProject()->HasOpenScene()); // There is no loaded scene.
    return core->LoadedProject()->GetOpenScene();
}

Scene::Scene(std::string_view name) noexcept :
    Name(name) {}
Scene::Scene(std::string&& name) noexcept :
    Name(std::move(name)) {}

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
size_t Scene::EntityCount() const { return _registry.storage<entt::entity>()->free_list(); }
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
    //auto& cam = GetActiveCamera();
    //cam.UpdateView();
    //cam.UpdateProjection();
    //cam.UpdateViewProjection();

    _registry.view<OrthoCameraComponent>().each([](Entity entt, OrthoCameraComponent& camera) {
        if (!camera.IsActiveAndRendering()) { return; }
        camera.UpdateMatrices();
        auto& fbo = camera.GetFrameBuffer();
        fbo.Bind();
        fbo.ClearBuffers();
        DOA_LOG_INFO("ortho camera attached to entity %d", EntityTo<int>(camera.GetEntity()));
        //render stuff here
    });

    _registry.view<PerspectiveCameraComponent>().each([](Entity entt, PerspectiveCameraComponent& camera) {
        if (!camera.IsActiveAndRendering()) { return; }
        camera.UpdateMatrices();
        auto& fbo = camera.GetFrameBuffer();
        fbo.Bind();
        fbo.ClearBuffers();
        DOA_LOG_INFO("perspective camera attached to entity %d", EntityTo<int>(camera.GetEntity()));
        //render stuff here
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