#include "Core.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <tinyxml2.h>

#include "Angel.hpp"
#include "Scene.hpp"
#include "Window.hpp"
#include "FrameBuffer.hpp"
#include "Log.hpp"
#include "ImGuiRenderer.hpp"
#include "Assets.hpp"
#include "Project.hpp"
#include "Resolution.hpp"
#include "Input.hpp"
#include "ProjectSerializer.hpp"
#include "ProjectDeserializer.hpp"

static void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

const CorePtr& Core::CreateCore(Resolution resolution, const char* title, bool isFullscreen, const char* windowIcon, bool renderOffscreen) {
#pragma region GLFW and Core/Window/Input Initialization
    glfwInit();
    _this = CorePtr(new Core, DeleteCore);
    _this->window = Window::CreateWindow(resolution, title, isFullscreen, windowIcon);
    _this->input = CreateInput();
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
    _this->angel = std::make_unique<struct Angel>();
#pragma endregion

#pragma region Built-in Stuff Initialization
    // TODO MOVE THIS INTO THE "DEFAULT RENDERER" CTOR (or INITER whatever)
    /*
    auto def = Texture::CreateTexture("!!default!!", "Images/default_texture.png");
    auto defx = Texture::CreateTexture("!!default_x!!", "Images/default_texture_x.png");
    auto defy = Texture::CreateTexture("!!default_y!!", "Images/default_texture_y.png");
    auto defz = Texture::CreateTexture("!!default_z!!", "Images/default_texture_z.png");
    auto miss = Texture::CreateTexture("!!missing!!", "Images/missing_texture.png");
    miss->Bind(); // TODO miss is dead after this function returns

    Shader::CreateFromFile("!!pick!!", "Shaders/mousePickVertexShader.vert", "Shaders/mousePickFragmentShader.frag");
    Shader::CreateFromFile("Simple Instanced Shader", "Shaders/simpleVertexShaderInstanced.vert", "Shaders/simpleFragmentShaderInstanced.frag");
    Shader::CreateFromFile("Simple Shader", "Shaders/simpleVertexShader.vert", "Shaders/simpleFragmentShader.frag");
    Shader::CreateFromFile("Solid Color Shader", "Shaders/solidColorVertexShader.vert", "Shaders/solidColorFragmentShader.frag");

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
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

    textures.push_back(defx);
    textures.push_back(defy);
    textures.push_back(defz);

    meshes.emplace_back(std::move(vertices), std::move(indices));

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

    meshes.emplace_back(std::move(vertices), std::move(indices));

    CreateModelFromMesh("Quad", std::move(meshes));
    vertices.clear();
    indices.clear();
    textures.clear();
    meshes.clear();
    */
#pragma endregion

    if (renderOffscreen) {
        FrameBufferBuilder builder;
        builder.SetResolution(resolution);
        builder.AddColorAttachment(OpenGL::RGB8);
        builder.SetDepthStencilAttachment(OpenGL::DEPTH24_STENCIL8);
        _this->offscreenBuffer = builder.BuildUnique();
    }
#pragma endregion

#pragma region KHR_debug
#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(message_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
#pragma endregion

    return _this;
}
const CorePtr& Core::GetCore() { return _this; }
void Core::DestroyCore() {
    _this->Stop();
    _this.reset();
}
void Core::DeleteCore(Core* core) { delete core; }

bool Core::IsRunning() const { return running; }
bool Core::IsPlaying() const { return playing; }
void Core::SetPlaying(bool playing) { this->playing = playing; }

std::unique_ptr<Angel>& Core::GetAngel() { return angel; }
WindowPtr& Core::GetWindow() { return window; }
std::unique_ptr<Input>& Core::GetInput() { return input; }
std::unique_ptr<FrameBuffer>& Core::GetFrameBuffer() { return offscreenBuffer; }

void Core::CreateAndLoadProject(std::string_view workspace, std::string_view name) {
    UnloadProject();
    project = std::make_unique<Project>(std::string(workspace), std::string(name));
    assets = std::make_unique<struct Assets>(*(project.get()));
    assets->EnsureDeserialization();
}
void Core::LoadProject(const std::string& path) {
    UnloadProject();

    FNode file({ .name = path });
    auto pdr = DeserializeProject(&file);
    if (pdr.erred) {
        DOA_LOG_FATAL("Could not deserialize project @%s", path.c_str());
        std::exit(1);
    }
    project = std::make_unique<Project>(std::move(pdr.project));
    assets = std::make_unique<struct Assets>(*project.get());
    assets->EnsureDeserialization();
    project->OpenStartupScene();
}
void Core::LoadProject(const std::filesystem::path& path) { LoadProject(path.string()); }
std::unique_ptr<Project>& Core::LoadedProject() { return project; }
void Core::UnloadProject() {
    playing = false;
    project.reset();
    assets.reset();
}
void Core::SaveLoadedProjectToDisk() const {
    tinyxml2::XMLDocument doc;
    doc.Parse(SerializeProject(*project.get()).c_str());
    doc.SaveFile((project->Workspace() / (project->Name() + ".doa")).string().c_str());
}
bool Core::HasLoadedProject() { return project != nullptr; }

std::unique_ptr<Assets>& Core::GetAssets() { return assets; }

void Core::Start() {
    static bool renderingOffscreen = offscreenBuffer != nullptr;
    float lastTime = static_cast<float>(glfwGetTime());
    float currentTime;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    running = true;
    while (running) {
        currentTime = static_cast<float>(glfwGetTime());
        glViewport(0, 0, window->GetContentResolution().Width, window->GetContentResolution().Height);

        float delta = currentTime - lastTime;

        if (project != nullptr && project->HasOpenScene()) {
            for (auto [id, attachment] : _attachments) {
                attachment->BeforeFrame(project.get());
            }
            Scene& scene = project->GetOpenScene();
            if (playing) {
                scene.Update(delta);
            }
            if (renderingOffscreen) {
                offscreenBuffer->Bind();
            }
            glClearColor(scene.ClearColor.r, scene.ClearColor.g, scene.ClearColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            scene.Render();
            if (renderingOffscreen) {
                FrameBuffer::BackBuffer().Bind();
                glViewport(0, 0, window->GetContentResolution().Width, window->GetContentResolution().Height);
            }
            for (auto [id, attachment] : _attachments) {
                attachment->AfterFrame(project.get());
            }
        }

        ImGuiRender(delta);

        glfwSwapBuffers(window->GetPlatformWindow());
        glfwPollEvents();
        lastTime = currentTime;

        if (glfwWindowShouldClose(window->GetPlatformWindow())) {
            Stop();
        }
    }
}
void Core::Stop() {
    running = false;
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