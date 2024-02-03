#include <Engine/ImGuiRenderer.hpp>

#include <vector>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stb_image.h>

#include <Engine/Window.hpp>
#include <Engine/FontAwesome.hpp>

static std::vector<ImGuiFunction> commands;
static ImGuiContext* context;

//- Data in-use for modifying newly created ImGui window icons -//
void (*ImGuiPlatformCreateWindow)(ImGuiViewport* vp);
std::vector<GLFWimage> ImGuiWindowIcons{};

ImGuiContext* ImGuiInit(GLFWwindow* window) {
    if (context != nullptr) { return context; }

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

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

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

    return context;
}
void ImGuiSetUpWindowIcons(std::vector<GLFWimage>&& icons) {
    ImGuiPlatformCreateWindow = ImGui::GetPlatformIO().Platform_CreateWindow;
    ImGuiWindowIcons = std::move(icons);
    ImGui::GetPlatformIO().Platform_CreateWindow = [](ImGuiViewport* viewport) {
        ImGuiPlatformCreateWindow(viewport);
        PlatformWindow* platformWindow = reinterpret_cast<PlatformWindow*>(viewport->PlatformHandle);
        glfwSetWindowIcon(platformWindow, ImGuiWindowIcons.size(), ImGuiWindowIcons.data());
    };
}

void ImGuiRender(float delta) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (const auto& command : commands) {
        command(delta);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    const ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiAddRenderCommand(ImGuiFunction function) {
    commands.emplace_back(function);
}

void ImGuiClean() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    for (auto& icon : ImGuiWindowIcons) {
        stbi_image_free(icon.pixels);
    }
}
