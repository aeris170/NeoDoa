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
#include "Assets.hpp"
#include "ScriptComponent.hpp"
#include "ModelRenderer.hpp"
#include "Project.hpp"
#include "Resolution.hpp"
#include "Input.hpp"

static void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

CorePtr& Core::CreateCore(Resolution resolution, const char* title, bool isFullscreen, const char* windowIcon, bool renderOffscreen) {
#pragma region GLFW and Core/Window/Input Initialization
    glfwInit();
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    _this = CorePtr(new Core, DeleteCore);
    _this->_window = Window::CreateWindow(resolution, title, isFullscreen, windowIcon);
    _this->_input = CreateInput();
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
    _this->_angel = std::make_unique<struct Angel>();
#pragma endregion

#pragma region Built-in Stuff Initialization
    std::weak_ptr<Texture> def = CreateTexture("!!default!!", "Images/default_texture.png");
    std::weak_ptr<Texture> defx = CreateTexture("!!default_x!!", "Images/default_texture_x.png");
    std::weak_ptr<Texture> defy = CreateTexture("!!default_y!!", "Images/default_texture_y.png");
    std::weak_ptr<Texture> defz = CreateTexture("!!default_z!!", "Images/default_texture_z.png");
    CreateTexture("!!missing!!", "Images/missing_texture.png").lock()->Bind();
    CreateShader("!!pick!!", "Shaders/mousePickVertexShader.vert", "Shaders/mousePickFragmentShader.frag");
    CreateShader("Simple Instanced Shader", "Shaders/simpleVertexShaderInstanced.vert", "Shaders/simpleFragmentShaderInstanced.frag");
    CreateShader("Simple Shader", "Shaders/simpleVertexShader.vert", "Shaders/simpleFragmentShader.frag");
    CreateShader("Solid Color Shader", "Shaders/solidColorVertexShader.vert", "Shaders/solidColorFragmentShader.frag");

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

    CreateModelFromMesh("Cube", std::move(meshes));
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

    CreateModelFromMesh("Quad", std::move(meshes));
    vertices.clear();
    indices.clear();
    textures.clear();
    meshes.clear();
#pragma endregion

    if (renderOffscreen) {
        _this->_offscreenBuffer = std::make_unique<struct FrameBuffer>(resolution);
    }
#pragma endregion

#pragma region KHR_debug
#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(message_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
#pragma endregion

    return _this;
}
CorePtr& Core::GetCore() { return _this; }
void Core::DestroyCore() {
    _this->Stop();
    _this.reset();
}
void Core::DeleteCore(Core* core) { delete core; }

bool Core::IsRunning() const { return _running; }
bool Core::IsPlaying() const { return _playing; }
void Core::SetPlaying(bool playing) { _playing = playing; }

std::unique_ptr<Angel>& Core::Angel() { return _angel; }
WindowPtr& Core::Window() { return _window; }
std::unique_ptr<Input>& Core::Input() { return _input; }
std::unique_ptr<FrameBuffer>& Core::FrameBuffer() { return _offscreenBuffer; }

void Core::LoadProject(const Project& project) { _project = &const_cast<Project&>(project); }
Project* Core::GetLoadedProject() const { return _project; }
void Core::UnloadProject() {
    _playing = false;
    _project = nullptr;
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

    _running = true;
    while (_running) {
        currentTime = glfwGetTime();
        glViewport(0, 0, _window->GetContentResolution().w, _window->GetContentResolution().h);

        float delta = currentTime - lastTime;

        if (_project != nullptr && _project->_openScene) {
            for (auto& [id, attachment] : _attachments) {
                attachment->BeforeFrame(_project);
            }
            Scene& scene = _project->_openScene.value();
            if (_playing) {
                scene.Update(delta);
            }
            if (renderingOffscreen) {
                _offscreenBuffer->Bind();
            }
            glClearColor(scene.ClearColor.r, scene.ClearColor.g, scene.ClearColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            scene.Render();
            if (renderingOffscreen) {
                _offscreenBuffer->UnBind(_window->GetContentResolution());
            }
            for (auto& [id, attachment] : _attachments) {
                attachment->AfterFrame(_project);
            }
        }

        ImGuiRender(delta);

        glfwSwapBuffers(_window->GetPlatformWindow());
        glfwPollEvents();
        lastTime = currentTime;

        if (glfwWindowShouldClose(_window->GetPlatformWindow())) {
            Stop();
        }
    }
}

void Core::Stop() {
    _running = false;
}

static void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param) {
    auto const src_str = [source]() {
        switch (source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        default: return "DEFAULT";
        }
    }();

    auto const type_str = [type]() {
        switch (type) {
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        case GL_DEBUG_TYPE_OTHER: return "OTHER";
        default: return "DEFAULT";
        }
    }();

    auto const severity_str = [severity]() {
        switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_LOW: return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        default: return "DEFAULT";
        }
    }();

    DOA_LOG_OPENGL("%s, %s, %s, %d: %s", src_str, type_str, severity_str, id, message);
}