#include <Engine/ImGuiRenderer.hpp>

#include <vector>
#include <utility>
#include <algorithm>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#ifdef SDL_SUPPORT
#include <imgui_impl_sdl2.h>
#endif
#if defined(OPENGL_4_6_SUPPORT) || defined(OPENGL_3_3_SUPPORT)
#include <imgui_impl_opengl3.h>
#endif
#if defined(VULKAN_SUPPORT)
#include <imgui_impl_vulkan.h>
#endif
#if defined(DIRECT3D_12_SUPPORT)
#include <imgui_impl_dx12.h>
#endif
#if defined(DIRECT3D_11_SUPPORT)
#include <imgui_impl_dx11.h>
#endif

#include <Engine/Core.hpp>
#include <Engine/Graphics.hpp>
#include <Engine/WindowGLFW.hpp>
#include <Engine/FontAwesome.hpp>

static std::vector<ImGuiFunction> commands;
static ImGuiContext* context;
static IWindow* window;

//- Data in-use for modifying newly created ImGui window icons -//
void (*ImGuiPlatformCreateWindow)(ImGuiViewport* vp);
WindowIconPack ImGuiWindowIcons{};

ImGuiContext* ImGuiInit(IWindow& window) {
    if (context != nullptr) { return context; }

    ::window = &window;

    IMGUI_CHECKVERSION();
    context = ImGui::CreateContext();

    ImGuiIO& io{ ImGui::GetIO() };
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;

    io.Fonts->AddFontFromFileTTF("Fonts/OpenSans-Regular.ttf", 18.0f, nullptr, ICONS_RANGES_TURKISH);
    io.Fonts->AddFontFromFileTTF("Fonts/FA6/fa-regular-400.ttf", 16.0f, &icons_config, icons_ranges);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Fonts/OpenSans-Bold.ttf", 18.0f, nullptr, ICONS_RANGES_TURKISH);
    io.Fonts->AddFontFromFileTTF("Fonts/FA6/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);


    switch (window.GetPlatformBackend()) {
    using enum WindowBackend;
    case GLFW:
        if (window.IsSoftwareRendererContextWindow()) {
            // TODO implement for software
        }
#if defined(OPENGL_4_6_SUPPORT) || defined(OPENGL_3_3_SUPPORT)
        else if (window.IsOpenGLContextWindow()) {
            ImGui_ImplGlfw_InitForOpenGL(std::any_cast<GLFWwindow*>(window.GetPlatformWindowPointer()), true);
        }
#endif
#ifdef VULKAN_SUPPORT
        else if (window.IsVulkanContextWindow()) {
            ImGui_ImplGlfw_InitForVulkan(std::any_cast<GLFWwindow*>(window.GetPlatformWindowPointer()), true);
        }
#endif
#ifdef DIRECT3D_12_SUPPORT
        else if (window.IsDirect3D12ContextWindow()) {
            ImGui_ImplGlfw_InitForOther(std::any_cast<GLFWwindow*>(window.GetPlatformWindowPointer()), true);
        }
#endif
#ifdef DIRECT3D_11_SUPPORT
        else if (window.IsDirect3D11ContextWindow()) {
            ImGui_ImplGlfw_InitForOther(std::any_cast<GLFWwindow*>(window.GetPlatformWindowPointer()), true);
        }
#endif
        break;
#ifdef SDL_SUPPORT
    case SDL:
        break;
#endif
    default:
        std::unreachable(); break;
    }

    if (window.IsSoftwareRendererContextWindow()) {
        // TODO implement for software
        assert(false);
    }
#if defined(OPENGL_4_6_SUPPORT) || defined(OPENGL_3_3_SUPPORT)
    else if (window.IsOpenGLContextWindow()) {
        ImGui_ImplOpenGL3_Init();
    }
#endif
#ifdef VULKAN_SUPPORT
    else if (window.IsVulkanContextWindow()) {
        // TODO implement for VK - take required fields from Graphics::Vulkan
        // ImGui_ImplVulkan_Init();
        assert(false);
    }
#endif
#ifdef DIRECT3D_12_SUPPORT
    else if (window.IsDirect3D12ContextWindow()) {
        // TODO implement for D3D - take required fields from Graphics::Direct3D12
        // ImGui_ImplDX12_Init();
        assert(false);
    }
#endif
#ifdef DIRECT3D_11_SUPPORT
    else if (window.IsDirect3D11ContextWindow()) {
        // TODO implement for D3D - take required fields from Graphics::Direct3D11
        // ImGui_ImplDX11_Init();
        assert(false);
    }
#endif

    //Courtesy of Yan Chernikov, aka The Cherno Project.
    { // Fancy lookin editor.
        ImGui::StyleColorsDark();

        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = WINDOW_BG;

        // Headers
        colors[ImGuiCol_Header] = IDLE;
        colors[ImGuiCol_HeaderHovered] = HOVER;
        colors[ImGuiCol_HeaderActive] = ACTIVE;

        // Buttons
        colors[ImGuiCol_Button] = IDLE;
        colors[ImGuiCol_ButtonHovered] = HOVER;
        colors[ImGuiCol_ButtonActive] = ACTIVE;

        // Frame BG
        colors[ImGuiCol_FrameBg] = IDLE;
        colors[ImGuiCol_FrameBgHovered] = HOVER;
        colors[ImGuiCol_FrameBgActive] = ACTIVE;

        // Tabs
        colors[ImGuiCol_Tab] = TAB_IDLE;
        colors[ImGuiCol_TabHovered] = TAB_HOVER;
        colors[ImGuiCol_TabActive] = TAB_ACTIVE;
        colors[ImGuiCol_TabUnfocused] = TAB_IDLE;
        colors[ImGuiCol_TabUnfocusedActive] = TAB_ACTIVE_NO_FOCUS;

        // Title
        colors[ImGuiCol_TitleBg] = TITLE;
        colors[ImGuiCol_TitleBgActive] = TITLE;
        colors[ImGuiCol_TitleBgCollapsed] = TITLE;

        // Misc.
        colors[ImGuiCol_CheckMark] = CHECKMARK;

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
    }

    ImGui::SetCurrentContext(context);
    return context;
}
void ImGuiSetUpWindowIcons(WindowIconPack icons) {
    ImGuiPlatformCreateWindow = ImGui::GetPlatformIO().Platform_CreateWindow;

    // Move (own the copy of) icons
    ImGuiWindowIcons = std::move(icons);

    ImGui::GetPlatformIO().Platform_CreateWindow = [](ImGuiViewport* viewport) {
        ImGuiPlatformCreateWindow(viewport);

        switch (window->GetPlatformBackend()) {
        using enum WindowBackend;
        case GLFW:
            WindowGLFW::SetIconOfPlatformWindow({ reinterpret_cast<WindowGLFW::PlatformWindowType>(viewport->PlatformHandle) }, ImGuiWindowIcons);
            break;
#ifdef SDL_SUPPORT
        case SDL:
            WindowSDL::SetIconOfPlatformWindow({ reinterpret_cast<WindowSDL::PlatformWindowType>(viewport->PlatformHandle) }, ImGuiWindowIcons);
            break;
#endif
        default:
            std::unreachable(); break;
        }
    };
}

void ImGuiRender(float delta) {
    if (window->IsSoftwareRendererContextWindow()) {
        // TODO implement for software
        assert(false);
    }
#if defined(OPENGL_4_6_SUPPORT) || defined(OPENGL_3_3_SUPPORT)
    else if (window->IsOpenGLContextWindow()) {
        ImGui_ImplOpenGL3_NewFrame();
    }
#endif
#ifdef VULKAN_SUPPORT
    else if (window->IsVulkanContextWindow()) {
        ImGui_ImplVulkan_NewFrame();
    }
#endif
#ifdef DIRECT3D_12_SUPPORT
    else if (window->IsDirect3D12ContextWindow()) {
        ImGui_ImplDX12_NewFrame();
    }
#endif
#ifdef DIRECT3D_11_SUPPORT
    else if (window->IsDirect3D11ContextWindow()) {
        ImGui_ImplDX11_NewFrame();
    }
#endif

    switch (window->GetPlatformBackend()) {
    using enum WindowBackend;
    case GLFW:
        ImGui_ImplGlfw_NewFrame();
        break;
#ifdef SDL_SUPPORT
    case SDL:
        ImGui_ImplSDL2_NewFrame();
        break;
#endif
    default:
        std::unreachable(); break;
    }

    if (window->IsSoftwareRendererContextWindow() ||
        window->IsOpenGLContextWindow() ||
        window->IsVulkanContextWindow() ||
        window->IsDirect3D12ContextWindow() ||
        window->IsDirect3D11ContextWindow()) {
        ImGui::NewFrame();

        for (const auto& command : commands) {
            command(delta);
        }

        ImGui::Render();
    }
    if (window->IsSoftwareRendererContextWindow()) {
        // TODO implement for software
        assert(false);
    }
#if defined(OPENGL_4_6_SUPPORT) || defined(OPENGL_3_3_SUPPORT)
    else if (window->IsOpenGLContextWindow()) {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
#endif
#ifdef VULKAN_SUPPORT
    else if (window->IsVulkanContextWindow()) {
        // TODO implement for VK - take required fields from Graphics::Vulkan
        // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
        assert(false);
    }
#endif
#ifdef DIRECT3D_12_SUPPORT
    else if (window->IsDirect3D12ContextWindow()) {
        // TODO implement for VK - take required fields from Graphics::Direct3D12
        // ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData());
        assert(false);
    }
#endif
#ifdef DIRECT3D_11_SUPPORT
    else if (window->IsDirect3D11ContextWindow()) {
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
#endif

    const ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        std::any backupCurrentContext = window->GetPlatformWindowPointer();
        if (Core::GetCore()->IsAnyContextInitialized()) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        switch (window->GetPlatformBackend()) {
        using enum WindowBackend;
        case GLFW:
            ImGui_ImplGlfw_NewFrame();
            break;
#ifdef SDL_SUPPORT
        case SDL:
            ImGui_ImplSDL2_NewFrame();
            break;
#endif
        default:
            std::unreachable(); break;
        }
        window->SetPlatformWindowPointerContext(backupCurrentContext);
    }
}

void ImGuiAddRenderCommand(ImGuiFunction function) {
    commands.emplace_back(function);
}

void ImGuiClean() {
    if (window->IsSoftwareRendererContextWindow()) {
        // TODO implement for software
        assert(false);
    }
#if defined(OPENGL_4_6_SUPPORT) || defined(OPENGL_3_3_SUPPORT)
    else if (window->IsOpenGLContextWindow()) {
        ImGui_ImplOpenGL3_Shutdown();
    }
#endif
#ifdef VULKAN_SUPPORT
    else if (window->IsVulkanContextWindow()) {
        ImGui_ImplVulkan_Shutdown();
    }
#endif
#ifdef DIRECT3D_12_SUPPORT
    else if (window->IsDirect3D12ContextWindow()) {
        ImGui_ImplDX12_Shutdown();
    }
#endif
#ifdef DIRECT3D_11_SUPPORT
    else if (window->IsDirect3D11ContextWindow()) {
        ImGui_ImplDX11_Shutdown();
    }
#endif

    switch (window->GetPlatformBackend()) {
        using enum WindowBackend;
    case GLFW:
        ImGui_ImplGlfw_Shutdown();
        break;
#ifdef SDL_SUPPORT
    case SDL:
        ImGui_ImplSDL2_Shutdown();
        break;
#endif
    default:
        std::unreachable(); break;
    }
    ImGui::DestroyContext();
    context = nullptr;
}
