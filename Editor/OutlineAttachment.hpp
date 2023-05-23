#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <Core.hpp>
#include <Project.hpp>
#include <FrameBuffer.hpp>
#include <Resolution.hpp>

#include <TransformComponent.hpp>
#include <MeshComponent.hpp>

#include "GUI.hpp"

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

