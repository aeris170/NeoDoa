#pragma once

#include <functional>

#include <Engine/Scene.hpp>

struct Observer;

struct SceneDisplay {

    SceneDisplay(Observer& observer) noexcept;

    void RenderEntities(const Scene& scene) const noexcept;
    void RenderSystems(const Scene& scene) const noexcept;

private:
    std::reference_wrapper<Observer> observer;
};