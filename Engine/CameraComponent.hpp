#pragma once

#include <memory>
#include <optional>

#include "ACamera.hpp"
#include "OrthoCamera.hpp"
#include "PerspectiveCamera.hpp"

#include "Resolution.hpp"
#include "Entity.hpp"

struct Scene;
namespace tinyxml2 {
    class XMLPrinter;
    class XMLElement;
}

struct OrthoCameraComponent {
private:
    Entity entity;

    bool isActiveAndRendering{ true };
    OrthoCamera data {
        -1.0f, 1.0f,
        -1.0f, 1.0f,
        -1.0f, 1.0f
    };

public:
    OrthoCameraComponent(const Entity owner) noexcept;
    OrthoCameraComponent(const Entity owner, const OrthoCamera& matrix) noexcept;

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
    Entity entity;

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