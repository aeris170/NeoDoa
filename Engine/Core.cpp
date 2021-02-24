#include "Core.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Angel.hpp"
#include "Scene.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include "ImGuiRenderer.hpp"

static std::unique_ptr<Core> _core;

std::unique_ptr<Core>& CreateCore(int width, int height, const char* title, bool isFullscreen, const char* windowIcon, bool renderOffscreen) {
#pragma region GLFW and Core/Window Initialization
    glfwInit();
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    _core = std::make_unique<Core>();
    _core->_window = std::make_unique<Window>(width, height, title, isFullscreen, windowIcon);
    _core->_angel = std::make_unique<Angel>();
#pragma endregion

#pragma region GLEW Initialization
    glewExperimental = GL_TRUE;
    glewInit();
#pragma endregion

#pragma region Built-in Stuff Initialization
    CreateTexture("!!missing!!", "Images/missing_texture.png").value().lock()->Bind(0);
    CreateShader("Simple Shader", "Shaders/simpleVertexShader.vert", "Shaders/simpleFragmentShader.frag");

    if (renderOffscreen) {
        _core->_offscreenBuffer = std::make_unique<FrameBuffer>(2000, 2000);
    }
#pragma endregion

    return _core;
}

void Core::Start() {
    static bool renderingOffscreen = _offscreenBuffer != nullptr;
    float lastTime = glfwGetTime();
    float currentTime;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_STENCIL_TEST);

    _running = true;
    while (_running) {
        currentTime = glfwGetTime();
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, _window->_width, _window->_height);

        auto scene = FindActiveScene().lock();
        if (_playing) {
            scene->Update(_angel, currentTime - lastTime);
        }
        if (renderingOffscreen) {
            glViewport(0, 0, 2000, 2000);
            glBindFramebuffer(GL_FRAMEBUFFER, _offscreenBuffer->_fbo);
            glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            scene->Render(_angel);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, _window->_width, _window->_height);
        } else {
            scene->Render(_angel);
        }
        ImGuiRender();

        glfwSwapBuffers(_window->_glfwWindow);
        glfwPollEvents();
        lastTime = currentTime;

        if (glfwWindowShouldClose(_window->_glfwWindow)) {
            Stop();
        }
    }
}

void Core::Stop() {
    _running = false;
}

std::unique_ptr<Core>& GetCore() {
    return _core;
}

void DestroyCore() {
    _core->Stop();
    _core.reset();
}
