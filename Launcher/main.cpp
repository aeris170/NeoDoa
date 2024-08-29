#include <stb_image.h>

#include <Engine/Log.hpp>
#include <Engine/Core.hpp>
#include <Engine/ImGuiRenderer.hpp>

#include <Launcher/GUI.hpp>

WindowIconPack::IconData LoadIcon(std::string_view path) noexcept {
    WindowIconPack::IconData data;
    int w, h, channels;

    stbi_uc* pixelData = stbi_load(path.data(), &w, &h, &channels, STBI_rgb_alpha);
    if (pixelData) {
        data.Size.Width = static_cast<unsigned>(w);
        data.Size.Height = static_cast<unsigned>(h);
        data.Pixels.assign(reinterpret_cast<std::add_pointer_t<RawData::value_type>>(pixelData),
                           reinterpret_cast<std::add_pointer_t<RawData::value_type>>(pixelData) + (w * h * STBI_rgb_alpha));
        stbi_image_free(pixelData);
    }
    return data;
}

int main() {
    //- Setup params -//
    WindowIconPack pack{};
    pack.Icons.emplace_back(LoadIcon("Images/launcherlogo-16_x_16.png"));
    pack.Icons.emplace_back(LoadIcon("Images/launcherlogo-32_x_32.png"));
    pack.Icons.emplace_back(LoadIcon("Images/launcherlogo-48_x_48.png"));
    pack.Icons.emplace_back(LoadIcon("Images/launcherlogo-64_x_64.png"));
    pack.Icons.emplace_back(LoadIcon("Images/launcherlogo-128_x_128.png"));
    pack.Icons.emplace_back(LoadIcon("Images/launcherlogo-256_x_256.png"));

    ContextWindowCreationParams params{
    .Size{ 800, 600 },
    .Title{ "NeoDoa Launcher" },

    .IsVisible{ true },
    .IsDecorated{ false },

    .Samples{ 8 },

    .IconPack{ pack },
    };
    //- Setup params -//

    DOA_LOG_INFO("Allocating %d bytes...", sizeof(Core));
    const CorePtr& core = Core::CreateCore(GraphicsBackend::OpenGL4_6, WindowBackend::GLFW, params);
    DOA_LOG_INFO("Core dynamically allocated!");

    DOA_LOG_INFO("Allocating %d bytes...", sizeof(GUI));
    std::shared_ptr<GUI> gui_ptr = std::make_shared<GUI>(core);
    DOA_LOG_INFO("GUI dynamically allocated!");
    ImGuiAddRenderCommand([gui = gui_ptr](float delta) { gui->operator()(delta); });

    core->Start();
    Core::DestroyCore();

    return 0;
}