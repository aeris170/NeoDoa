#include "Core.hpp"

#include <utility>

#include <GLFW/glfw3.h>
#include <tinyxml2.h>

#include "Angel.hpp"
#include "Scene.hpp"
#include "Window.hpp"
#include "Log.hpp"
#include "ImGuiRenderer.hpp"
#include "Assets.hpp"
#include "Project.hpp"
#include "Resolution.hpp"
#include "Input.hpp"
#include "ProjectSerializer.hpp"
#include "ProjectDeserializer.hpp"

#include <Engine/Monitor.hpp>
#include <Engine/WindowGLFW.hpp>

const CorePtr& Core::CreateCore(GraphicsBackend gBackend, WindowBackend wBackend, const ContextWindowCreationParams& params) {
    std::set_new_handler(HandleNew);
#pragma region Core/Window/Input/ImGui Initialization
    Monitors::Initialize();
    CorePtr& core = *const_cast<CorePtr*>(&GetCore()); // cast-away const to initialize internals. GetCore never instantiates a const Core!

    switch (wBackend) {
        using enum WindowBackend;
        case GLFW:
            core->window = std::make_unique<WindowGLFW>(gBackend, params);
            break;
#ifdef SDL_SUPPORT
        case SDL:
            core->window = std::make_unique<WindowSDL>(gBackend, params);
            break;
#endif
        default:
            std::unreachable();
    };

    IWindow& window = *core->window.get();
    core->input = std::make_unique<Input>(window);
    ImGuiInit(window);
    ImGuiSetUpWindowIcons(params.IconPack);
#pragma endregion

#pragma region GLEW Initialization
    if (window.IsSoftwareRendererContextWindow() || window.IsOpenGLContextWindow()) {
        glewExperimental = GL_TRUE;
        GLenum error = glewInit();
        if (error != GLEW_OK) {
            DOA_LOG_FATAL("GLEW couldn't initialize! Reason: %s", glewGetErrorString(error));
            std::abort();
        }
    }
#pragma endregion

#pragma region Graphics Backend Initialization
    Graphics::ChangeGraphicsBackend(gBackend);
#pragma endregion

#pragma region Angel Initialization
    core->angel = std::make_unique<Angel>();
#pragma endregion

#pragma region GPU Resource Allocator Initialization
    core->gpuBridge = std::make_unique<AssetGPUBridge>();
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

    return core;
}
const CorePtr& Core::GetCore() {
    static CorePtr core{ new Core, DeleteCore };
    return core;
}
void Core::DestroyCore() {
    Monitors::DeInitialize();
    ImGuiClean();
    CorePtr& core = *const_cast<CorePtr*>(&GetCore()); // cast-away const to destroy instance. GetCore never instantiates a const Core!
    core->Stop();
    core.reset();
}

bool Core::IsAnyContextInitialized() const noexcept {
    return window &&
        (window->IsSoftwareRendererContextWindow() ||
         window->IsOpenGLContextWindow() ||
         window->IsVulkanContextWindow() ||
         window->IsDirect3D12ContextWindow() ||
         window->IsDirect3D11ContextWindow());
}
bool Core::IsSoftwareRendererInitialized() const noexcept {
    return window && window->IsSoftwareRendererContextWindow();
}
#if defined(OPENGL_4_6_SUPPORT) || defined(OPENGL_3_3_SUPPORT)
bool Core::IsOpenGLInitialized() const noexcept {
    return window && window->IsOpenGLContextWindow();
}
#endif
#ifdef VULKAN_SUPPORT
bool Core::IsVulkanInitialized() const noexcept {
    return window && window->IsVulkanContextWindow();
}
#endif
#ifdef DIRECT3D_12_SUPPORT
bool Core::IsDirect3D12Initialized() const noexcept {
    return window && window->IsDirect3D12ContextWindow();
}
#endif
#ifdef DIRECT3D_11_SUPPORT
bool Core::IsDirect3D11Initialized() const noexcept {
    return window && window->IsDirect3D11ContextWindow();
}
#endif

bool Core::IsRunning() const { return running; }
bool Core::IsPlaying() const { return playing; }
void Core::SetPlaying(bool playing) { this->playing = playing; }

std::unique_ptr<Angel>& Core::GetAngel() { return angel; }
std::unique_ptr<IWindow>& Core::GetWindow() { return window; }
std::unique_ptr<Input>& Core::GetInput() { return input; }

void Core::CreateAndLoadProject(std::string_view workspace, std::string_view name) {
    UnloadProject();

    gpuBridge = std::make_unique<AssetGPUBridge>();
    project = std::make_unique<Project>(std::string(workspace), std::string(name));
    assets = std::make_unique<Assets>(*project.get(), *gpuBridge.get());
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
    gpuBridge = std::make_unique<AssetGPUBridge>();
    project = std::make_unique<Project>(std::move(pdr.project));
    assets = std::make_unique<Assets>(*project.get(), *gpuBridge.get());
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
std::unique_ptr<AssetGPUBridge>& Core::GetAssetGPUBridge() { return gpuBridge; }

void Core::Start() {
    // TODO get rid of glfwGetTime here!
    float lastTime = static_cast<float>(glfwGetTime());
    float currentTime;

    running = true;
    while (running) {
        // TODO get rid of glfwGetTime here!
        currentTime = static_cast<float>(glfwGetTime());

        float delta = currentTime - lastTime;

        if (project != nullptr && project->HasOpenScene()) {
            for (auto [id, attachment] : _attachments) {
                attachment->BeforeFrame(project.get());
            }

            Scene& scene = project->GetOpenScene();
            scene.ExecuteSystems(playing, delta);

            for (auto [id, attachment] : _attachments) {
                attachment->AfterFrame(project.get());
            }
        }

        ImGuiRender(delta);

        window->SwapBuffers();

        input->Step();
        window->PollEvents();
        lastTime = currentTime;

        if (window->ShouldClose()) { Stop(); }
    }
}
void Core::Stop() {
    running = false;
}

void Core::DeleteCore(Core* core) { delete core; }
void Core::HandleNew() {
    DOA_LOG_FATAL("Memory allocation failed, terminating");
    std::set_new_handler(nullptr);
}