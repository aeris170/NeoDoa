#include <Engine/Scene.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <Engine/Core.hpp>

#include <Engine/Log.hpp>
#include <Engine/Texture.hpp>
#include <Engine/TransformComponent.hpp>
#include <Engine/IDComponent.hpp>
#include <Engine/ParentComponent.hpp>
#include <Engine/ChildComponent.hpp>
#include <Engine/CameraComponent.hpp>
#include <Engine/Project.hpp>
#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>

Scene& Scene::GetLoadedScene() {
    static auto& core = Core::GetCore();
    assert(core->LoadedProject() != nullptr); // There is no loaded project, hence no loaded scene.
    assert(core->LoadedProject()->HasOpenScene()); // There is no loaded scene.
    return core->LoadedProject()->GetOpenScene();
}

Scene::Scene(std::string_view name) noexcept :
    Name(name) {}

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
        ParentComponent& parent = GetComponent<ParentComponent>(entt);
        for (const auto& child : parent.GetChildren()) {
            DeleteEntity(child);
        }
    }

    if (HasComponent<ChildComponent>(entt)) {
        const ChildComponent& child = GetComponent<ChildComponent>(entt);
        assert(HasComponent<ParentComponent>(child.GetParent()));
        ParentComponent& parent = GetComponent<ParentComponent>(child.GetParent());
        auto search = std::ranges::find(parent.GetChildren(), entt);
        assert(search != parent.GetChildren().end());
        parent.GetChildren().erase(search);
        if (parent.GetChildren().empty()) {
            RemoveComponent<ParentComponent>(child.GetParent());
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

void Scene::ExecuteSystems(bool isPlaying, float deltaTime) {
    for (entt::poly<System>& s : _systems) {
        s->Init(_registry);
        s->Execute(_registry, deltaTime);
    }
}