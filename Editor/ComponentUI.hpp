#pragma once

#include <string_view>

struct GUI;

struct IDComponent;
namespace IDComponentUI {
    void Render(GUI& gui, const IDComponent& idComponent);
};

struct TransformComponent;
namespace TransformComponentUI {
    void Render(GUI& gui, const TransformComponent& transformComponent);
}

struct ParentComponent;
namespace ParentComponentUI {
    void Render(const ParentComponent& parentComponent);
}

struct ChildComponent;
namespace ChildComponentUI {
    void Render(const ChildComponent& childComponent);
};

struct MultiMaterialComponent;
namespace MultiMaterialComponentUI {
    void Render(GUI& gui, const MultiMaterialComponent& multiMaterialComponent);
};

struct OrthoCameraComponent;
namespace OrthoCameraComponentUI {
    void Render(const OrthoCameraComponent& orthoCameraComponent);
};

struct PerspectiveCameraComponent;
namespace PerspectiveCameraComponentUI {
    void Render(const PerspectiveCameraComponent& perspectiveCameraComponent);
};


struct Assets;
struct ComponentInstance;
struct UserDefinedComponentStorage;
namespace UserDefinedComponentStorageUI {
    void RenderComponentInstance(const ComponentInstance& instance);
}

struct Scene;
struct Observer;
namespace ComponentUI {
    bool Begin(const Observer& observer, std::string_view componentTypeName);
    bool Begin(const Observer& observer, const ComponentInstance& instance);

    void RenderIDComponent(const Observer& observer, const IDComponent& idComponent);
    void RenderTransformComponent(const Observer& observer, const TransformComponent& transformComponent);
    void RenderParentComponent(const Observer& observer, const ParentComponent& parentComponent);
    void RenderChildComponent(const Observer& observer, const ChildComponent& childComponent);
    void RenderMultiMaterialComponent(const Observer& observer, const MultiMaterialComponent& multiMaterialComponent);
    void RenderOrthoCameraComponent(const Observer& observer, const OrthoCameraComponent& orthoCameraComponent);
    void RenderPerspectiveCameraComponent(const Observer& observer, const PerspectiveCameraComponent& perspectiveCameraComponent);
    void RenderUserDefinedComponentStorage(const Observer& observer, const UserDefinedComponentStorage& storageComponent);
    void End(bool show);
};