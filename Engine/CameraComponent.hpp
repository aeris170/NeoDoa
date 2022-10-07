#pragma once

#include <memory>
#include <optional>

#include "ACamera.hpp"
#include "OrthoCamera.hpp"
#include "PerspectiveCamera.hpp"

#include "Resolution.hpp"
#include "Entity.hpp"
#include "FrameBuffer.hpp"

struct Scene;
namespace tinyxml2 {
    class XMLPrinter;
    class XMLElement;
}

struct OrthoCameraComponent {
private:
    Entity entity;

    bool isActiveAndRendering{ true };
    OrthoCamera data{
        -1.0f, 1.0f,
        -1.0f, 1.0f,
        -1.0f, 1.0f
    };
    FrameBuffer frameBuffer{ Resolution{ 1920, 1080 } };

    OrthoCameraComponent(const Entity owner, OrthoCamera&& matrix) noexcept;

public:
    OrthoCameraComponent(const Entity owner) noexcept;

    Entity GetEntity() const;

    const OrthoCamera& GetData() const;
    void SetData(OrthoCamera&& data);

    void UpdateMatrices();

    void TurnOn();
    void TurnOff();
    bool IsActiveAndRendering() const;

    FrameBuffer& GetFrameBuffer();
    const FrameBuffer& GetFrameBuffer() const;
    void SetUpFrameBuffer(Resolution resolution);

    friend void SerializeOrthoCameraComponent(tinyxml2::XMLPrinter& printer, const OrthoCameraComponent& camera);
    friend OrthoCameraComponent DeserializeOrthoCameraComponent(tinyxml2::XMLElement* component, const Entity entity, const Scene& scene);
};

struct PerspectiveCameraComponent {
private:
    Entity entity;

    bool isActiveAndRendering{ true };
    PerspectiveCamera data{
        110,
        16.0f / 9.0f,
        0.001f,
        10000.0f
    };
    FrameBuffer frameBuffer{ Resolution{ 1920, 1080 } };

    PerspectiveCameraComponent(const Entity owner, PerspectiveCamera&& data) noexcept;

public:
    PerspectiveCameraComponent(const Entity owner) noexcept;

    Entity GetEntity() const;

    const PerspectiveCamera& GetData() const;
    void SetData(PerspectiveCamera&& data);

    void UpdateMatrices();

    void TurnOn();
    void TurnOff();
    bool IsActiveAndRendering() const;

    FrameBuffer& GetFrameBuffer();
    const FrameBuffer& GetFrameBuffer() const;
    void SetUpFrameBuffer(Resolution resolution);

    friend void SerializePerspectiveCameraComponent(tinyxml2::XMLPrinter& printer, const PerspectiveCameraComponent& camera);
    friend PerspectiveCameraComponent DeserializePerspectiveCameraComponent(tinyxml2::XMLElement* component, const Entity entity, const Scene& scene);
};