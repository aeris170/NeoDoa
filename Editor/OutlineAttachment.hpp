#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <Engine/Core.hpp>
#include <Engine/Project.hpp>
#include <Engine/FrameBuffer.hpp>
#include <Engine/Resolution.hpp>
#include <Engine/TransformComponent.hpp>
#include <Engine/MeshComponent.hpp>

#include <Editor/GUI.hpp>

struct OutlineAttachment {

    std::shared_ptr<GUI> gui;
    glm::vec4 OutlineColor{ 1.0f, 1.0f, 1.0f, 1.0f };
    FrameBuffer fbo;
    std::weak_ptr<Shader> solidColorShader;
    std::weak_ptr<Shader> gaussianBlurShader;

    OutlineAttachment(std::shared_ptr<GUI> gui) noexcept;

    void BeforeFrame(Project* project);
    void AfterFrame(Project* project);
};

