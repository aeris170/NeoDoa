#include <vld.h>

#include <Core.hpp>
#include <ImGuiRenderer.hpp>

#include "GUI.hpp"
#include "OutlineAttachment.hpp"

int main() {
    DOA_LOG_INFO("Allocating %d bytes...", sizeof(Core));
    const CorePtr& core = Core::CreateCore({ 2000, 2000 }, "NeoDoa Editor", false, "Images/neodoalogo", true);
    DOA_LOG_INFO("Core dynamically allocated!");

    DOA_LOG_INFO("Allocating %d bytes...", sizeof(GUI));
    std::shared_ptr<GUI> gui_ptr = std::make_shared<GUI>(core);
    DOA_LOG_INFO("GUI dynamically allocated!");
    ImGuiAddRenderCommand([gui = gui_ptr](float delta) { gui->operator()(delta); });

    core->CreateAttachment<OutlineAttachment>(gui_ptr);
    core->Start();
    Core::DestroyCore();

    return 0;
}