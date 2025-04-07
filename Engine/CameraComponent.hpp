#pragma once

#include <memory>
#include <optional>

#include <Engine/ACamera.hpp>
#include <Engine/OrthoCamera.hpp>
#include <Engine/PerspectiveCamera.hpp>

#include <Engine/Resolution.hpp>
#include <Engine/ECSComponent.hpp>

struct Scene;
namespace tinyxml2 {
    class XMLPrinter;
    class XMLElement;
}

struct OrthoCameraComponent {
private:
    Entity entity{ NULL_ENTT };

    bool isActiveAndRendering{ true };
    OrthoCamera data {
        -1.0f, 1.0f,
        -1.0f, 1.0f,
        -1.0f, 1.0f
    };

public:
    OrthoCameraComponent(const Entity owner) noexcept;
    OrthoCameraComponent(const Entity owner, const OrthoCamera& matrix) noexcept;
    ~OrthoCameraComponent() noexcept = default;
    OrthoCameraComponent(const OrthoCameraComponent& other) noexcept;
    OrthoCameraComponent(OrthoCameraComponent&& other) noexcept;
    OrthoCameraComponent& operator=(const OrthoCameraComponent& other) noexcept;
    OrthoCameraComponent& operator=(OrthoCameraComponent&& other) noexcept;

    Entity GetEntity() const;

    const OrthoCamera& GetData() const noexcept;
    void SetData(const OrthoCamera& data) noexcept;

    void UpdateMatrices();

    void TurnOn();
    void TurnOff();
    bool IsActiveAndRendering() const;

    friend void SerializeOrthoCameraComponent(tinyxml2::XMLPrinter& printer, const OrthoCameraComponent& camera);
    friend OrthoCameraComponent DeserializeOrthoCameraComponent(tinyxml2::XMLElement* component, const Entity entity, const Scene& scene);
};

struct PerspectiveCameraComponent {
private:
    Entity entity{ NULL_ENTT };

    bool isActiveAndRendering{ true };
    PerspectiveCamera data {
        110,
        16.0f / 9.0f,
        0.001f,
        10000.0f
    };

public:
    PerspectiveCameraComponent(const Entity owner) noexcept;
    PerspectiveCameraComponent(const Entity owner, const PerspectiveCamera& data) noexcept;
    ~PerspectiveCameraComponent() noexcept = default;
    PerspectiveCameraComponent(const PerspectiveCameraComponent& other) noexcept;
    PerspectiveCameraComponent(PerspectiveCameraComponent&& other) noexcept;
    PerspectiveCameraComponent& operator=(const PerspectiveCameraComponent& other) noexcept;
    PerspectiveCameraComponent& operator=(PerspectiveCameraComponent&& other) noexcept;

    Entity GetEntity() const;

    const PerspectiveCamera& GetData() const noexcept;
    void SetData(const PerspectiveCamera& data) noexcept;

    void UpdateMatrices();

    void TurnOn();
    void TurnOff();
    bool IsActiveAndRendering() const;

    friend void SerializePerspectiveCameraComponent(tinyxml2::XMLPrinter& printer, const PerspectiveCameraComponent& camera);
    friend PerspectiveCameraComponent DeserializePerspectiveCameraComponent(tinyxml2::XMLElement* component, const Entity entity, const Scene& scene);
};

static_assert(ECSComponent<OrthoCameraComponent>);
static_assert(ECSComponent<PerspectiveCameraComponent>);