#include <Engine/Core.hpp>
#include <Engine/ImGuiRenderer.hpp>

#include <Launcher/GUI.hpp>

int main() {
    DOA_LOG_INFO("Allocating %d bytes...", sizeof(Core));
    const CorePtr& core = Core::CreateCore({ 800, 600 }, "NeoDoa Launcher", false, "Images/launcherlogo");
    DOA_LOG_INFO("Core dynamically allocated!");

    core->Window()->SetDecorated(false);

    DOA_LOG_INFO("Allocating %d bytes...", sizeof(GUI));
    std::shared_ptr<GUI> gui_ptr = std::make_shared<GUI>(core);
    DOA_LOG_INFO("GUI dynamically allocated!");
    ImGuiAddRenderCommand([gui = gui_ptr](float delta) { gui->operator()(delta); });

    core->Start();
    Core::DestroyCore();

    return 0;
}