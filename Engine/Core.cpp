#include "Core.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Angel.hpp"
#include "Scene.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include "Vertex.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Log.hpp"
#include "ImGuiRenderer.hpp"
#include "AssetManager.hpp"

static std::unique_ptr<Core> _core;

std::unique_ptr<Core>& CreateCore(int width, int height, const char* title, bool isFullscreen, const char* windowIcon, bool renderOffscreen) {
#pragma region GLFW and Core/Window Initialization
    glfwInit();
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    _core = std::make_unique<Core>();
    _core->_window = std::make_unique<Window>(width, height, title, isFullscreen, windowIcon);
#pragma endregion

#pragma region GLEW Initialization
    glewExperimental = GL_TRUE;
    glewInit();
#pragma endregion

#pragma region System Information
    DOA_LOG_INFO("OpenGL version: %s", glGetString(GL_VERSION));
    DOA_LOG_INFO("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    DOA_LOG_INFO("Vendor: %s", glGetString(GL_VENDOR));
    DOA_LOG_INFO("GPU: %s", glGetString(GL_RENDERER));
#pragma endregion

#pragma region Angel Initialization
    _core->_angel = std::make_unique<Angel>();
#pragma endregion

#pragma region Built-in Stuff Initialization
    std::weak_ptr<Texture> def = CreateTexture("!!default!!", "Images/default_texture.png").value();
    std::weak_ptr<Texture> defx = CreateTexture("!!default_x!!", "Images/default_texture_x.png").value();
    std::weak_ptr<Texture> defy = CreateTexture("!!default_y!!", "Images/default_texture_y.png").value();
    std::weak_ptr<Texture> defz = CreateTexture("!!default_z!!", "Images/default_texture_z.png").value();
    CreateTexture("!!missing!!", "Images/missing_texture.png").value().lock()->Bind(0);
    CreateShader("Simple Shader", "Shaders/simpleVertexShader.vert", "Shaders/simpleFragmentShader.frag");

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<std::weak_ptr<Texture>> textures;
    std::vector<Mesh> meshes;
#pragma region Cube
    // front face
    vertices.emplace_back(Vertex{ { -1, -1,  1 }, {  0,  0,  1 }, {  1,  1,  1,  1 }, {  0,  0 }, 2 });
    vertices.emplace_back(Vertex{ {  1, -1,  1 }, {  0,  0,  1 }, {  1,  1,  1,  1 }, {  1,  0 }, 2 });
    vertices.emplace_back(Vertex{ {  1,  1,  1 }, {  0,  0,  1 }, {  1,  1,  1,  1 }, {  1,  1 }, 2 });
    vertices.emplace_back(Vertex{ { -1,  1,  1 }, {  0,  0,  1 }, {  1,  1,  1,  1 }, {  0,  1 }, 2 });

    // back face
    vertices.emplace_back(Vertex{ { -1, -1, -1 }, {  0,  0, -1 }, {  1,  1,  1,  1 }, {  0,  0 }, 2 });
    vertices.emplace_back(Vertex{ {  1, -1, -1 }, {  0,  0, -1 }, {  1,  1,  1,  1 }, {  1,  0 }, 2 });
    vertices.emplace_back(Vertex{ {  1,  1, -1 }, {  0,  0, -1 }, {  1,  1,  1,  1 }, {  1,  1 }, 2 });
    vertices.emplace_back(Vertex{ { -1,  1, -1 }, {  0,  0, -1 }, {  1,  1,  1,  1 }, {  0,  1 }, 2 });

    // top face
    vertices.emplace_back(Vertex{ { -1,  1,  1 }, {  0,  1,  0 }, {  1,  1,  1,  1 }, {  0,  0 }, 1 });
    vertices.emplace_back(Vertex{ {  1,  1,  1 }, {  0,  1,  0 }, {  1,  1,  1,  1 }, {  1,  0 }, 1 });
    vertices.emplace_back(Vertex{ {  1,  1, -1 }, {  0,  1,  0 }, {  1,  1,  1,  1 }, {  1,  1 }, 1 });
    vertices.emplace_back(Vertex{ { -1,  1, -1 }, {  0,  1,  0 }, {  1,  1,  1,  1 }, {  0,  1 }, 1 });

    // bottom face
    vertices.emplace_back(Vertex{ { -1, -1,  1 }, {  0, -1,  0 }, {  1,  1,  1,  1 }, {  0,  0 }, 1 });
    vertices.emplace_back(Vertex{ {  1, -1,  1 }, {  0, -1,  0 }, {  1,  1,  1,  1 }, {  1,  0 }, 1 });
    vertices.emplace_back(Vertex{ {  1, -1, -1 }, {  0, -1,  0 }, {  1,  1,  1,  1 }, {  1,  1 }, 1 });
    vertices.emplace_back(Vertex{ { -1, -1, -1 }, {  0, -1,  0 }, {  1,  1,  1,  1 }, {  0,  1 }, 1 });

    // right face
    vertices.emplace_back(Vertex{ {  1, -1,  1 }, {  1,  0,  0 }, {  1,  1,  1,  1 }, {  0,  0 }, 0 });
    vertices.emplace_back(Vertex{ {  1, -1, -1 }, {  1,  0,  0 }, {  1,  1,  1,  1 }, {  1,  0 }, 0 });
    vertices.emplace_back(Vertex{ {  1,  1, -1 }, {  1,  0,  0 }, {  1,  1,  1,  1 }, {  1,  1 }, 0 });
    vertices.emplace_back(Vertex{ {  1,  1,  1 }, {  1,  0,  0 }, {  1,  1,  1,  1 }, {  0,  1 }, 0 });

    // left face
    vertices.emplace_back(Vertex{ { -1, -1,  1 }, { -1,  0,  0 }, {  1,  1,  1,  1 }, {  0,  0 }, 0 });
    vertices.emplace_back(Vertex{ { -1, -1, -1 }, { -1,  0,  0 }, {  1,  1,  1,  1 }, {  1,  0 }, 0 });
    vertices.emplace_back(Vertex{ { -1,  1, -1 }, { -1,  0,  0 }, {  1,  1,  1,  1 }, {  1,  1 }, 0 });
    vertices.emplace_back(Vertex{ { -1,  1,  1 }, { -1,  0,  0 }, {  1,  1,  1,  1 }, {  0,  1 }, 0 });

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    indices.push_back(6);
    indices.push_back(5);
    indices.push_back(4);
    indices.push_back(4);
    indices.push_back(7);
    indices.push_back(6);

    indices.push_back(8);
    indices.push_back(9);
    indices.push_back(10);
    indices.push_back(10);
    indices.push_back(11);
    indices.push_back(8);

    indices.push_back(14);
    indices.push_back(13);
    indices.push_back(12);
    indices.push_back(12);
    indices.push_back(15);
    indices.push_back(14);

    indices.push_back(16);
    indices.push_back(17);
    indices.push_back(18);
    indices.push_back(18);
    indices.push_back(19);
    indices.push_back(16);

    indices.push_back(22);
    indices.push_back(21);
    indices.push_back(20);
    indices.push_back(20);
    indices.push_back(23);
    indices.push_back(22);

    textures.emplace_back(defx);
    textures.emplace_back(defy);
    textures.emplace_back(defz);

    meshes.emplace_back(std::move(vertices), std::move(indices), std::move(textures));

    CreateModelFromMesh("Cube", meshes);
    vertices.clear();
    indices.clear();
    textures.clear();
    meshes.clear();
#pragma endregion
#pragma region Quad
    vertices.emplace_back(Vertex{ { -1, -1,  0 }, {  0,  0,  1 }, {  1,  1,  1,  1 }, {  0,  0 }, 0 });
    vertices.emplace_back(Vertex{ {  1, -1,  0 }, {  0,  0,  1 }, {  1,  1,  1,  1 }, {  1,  0 }, 0 });
    vertices.emplace_back(Vertex{ {  1,  1,  0 }, {  0,  0,  1 }, {  1,  1,  1,  1 }, {  1,  1 }, 0 });
    vertices.emplace_back(Vertex{ { -1,  1,  0 }, {  0,  0,  1 }, {  1,  1,  1,  1 }, {  0,  1 }, 0 });

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    textures.push_back(def);

    meshes.emplace_back(std::move(vertices), std::move(indices), std::move(textures));

    CreateModelFromMesh("Quad", meshes);
    vertices.clear();
    indices.clear();
    textures.clear();
    meshes.clear();
#pragma endregion

    if (renderOffscreen) {
        _core->_offscreenBuffer = std::make_unique<FrameBuffer>(1920, 1080);
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
        glViewport(0, 0, _window->_content_width, _window->_content_height);

        auto scenePtr = FindActiveScene();
        auto scene = scenePtr.lock();
        float delta = currentTime - lastTime;
        if (_playing) {
            if (!scenePtr.expired()) {
                scene->Update(_angel, delta);
            }
        }
        if (!scenePtr.expired()) {
            if (renderingOffscreen) {
                glViewport(0, 0, _offscreenBuffer->_width, _offscreenBuffer->_height);
                glBindFramebuffer(GL_FRAMEBUFFER, _offscreenBuffer->_fbo);
            }
            glClearColor(scene->ClearColor.x, scene->ClearColor.y, scene->ClearColor.z, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            if (!scenePtr.expired()) {
                scene->Render(_angel);
            }
            if (renderingOffscreen) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(0, 0, _window->_content_width, _window->_content_height);
            }
        }
        ImGuiRender(delta);

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
    DeleteAllScenes();
    //DeleteAll

    _core->Stop();
    _core.reset();
}
