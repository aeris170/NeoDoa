#pragma once

#include <string_view>

struct IDComponent;
namespace IDComponentUI {
    void Render(const IDComponent& idComponent);
};

struct TransformComponent;
namespace TransformComponentUI {
    void Render(const TransformComponent& transformComponent);
}

struct ParentComponent;
namespace ParentComponentUI {
    void Render(const ParentComponent& parentComponent);
}

struct ChildComponent;
namespace ChildComponentUI {
    void Render(const ChildComponent& childComponent);
};

struct OrthoCameraComponent;
namespace OrthoCameraComponentUI {
    void Render(const OrthoCameraComponent& orthoCameraComponent);
};

struct PerspectiveCameraComponent;
namespace PerspectiveCameraComponentUI {
    void Render(const PerspectiveCameraComponent& perspectiveCameraComponent);
};

struct ScriptStorageComponent;

struct Scene;
struct Observer;
namespace ComponentUI {
    bool Begin(const Observer& observer, std::string_view componentTypeName);

    void RenderIDComponent(const Observer& observer, Scene& scene, IDComponent& idComponent);
    void RenderTransformComponent(const Observer& observer, Scene& scene, TransformComponent& transformComponent);
    void RenderParentComponent(const Observer& observer, Scene& scene, ParentComponent& parentComponent);
    void RenderChildComponent(const Observer& observer, Scene& scene, ChildComponent& childComponent);
    void RenderOrthoCameraComponent(const Observer& observer, Scene& scene, OrthoCameraComponent& orthoCameraComponent);
    void RenderPerspectiveCameraComponent(const Observer& observer, Scene& scene, PerspectiveCameraComponent& perspectiveCameraComponent);
    void RenderScriptStorageComponent(const Observer& observer, Scene& scene, ScriptStorageComponent& scriptStorageComponent);

    void End(bool show);
};