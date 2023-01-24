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

    void RenderScriptStorageComponent(const Observer& observer, Scene& scene, ScriptStorageComponent& scriptStorageComponent);

    void RenderIDComponent(const Observer& observer, const IDComponent& idComponent);
    void RenderTransformComponent(const Observer& observer, const TransformComponent& transformComponent);
    void RenderParentComponent(const Observer& observer, const ParentComponent& parentComponent);
    void RenderChildComponent(const Observer& observer, const ChildComponent& childComponent);
    void RenderOrthoCameraComponent(const Observer& observer, const OrthoCameraComponent& orthoCameraComponent);
    void RenderPerspectiveCameraComponent(const Observer& observer, const PerspectiveCameraComponent& perspectiveCameraComponent);
    void End(bool show);
};