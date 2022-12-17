#include <Core.hpp>
#include <ImGuiRenderer.hpp>

#include "GUI.hpp"
#include "OutlineAttachment.hpp"

#include <iostream>
#include <vld.h>
int main() {
    std::cout << "Allocating " << sizeof(Core) << " bytes..." << std::endl;
    const CorePtr& core = Core::CreateCore({ 2000, 2000 }, "NeoDoa Editor", false, "Images/neodoalogo", true);
    std::cout << "Core dynamically allocated!" << std::endl;

    std::cout << "Allocating " << sizeof(GUI) << " bytes." << std::endl;
    std::shared_ptr<GUI> gui_ptr = std::make_shared<GUI>(core);
    std::cout << "GUI dynamically allocated!" << std::endl;
    ImGuiAddRenderCommand([gui = gui_ptr](float delta) { gui->operator()(delta); });

    core->CreateAttachment<OutlineAttachment>(gui_ptr);
    core->Start();
    Core::DestroyCore();

    return 0;
}