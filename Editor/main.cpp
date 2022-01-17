#include <Core.hpp>
#include <Window.hpp>
#include <ImGuiRenderer.hpp>

#include "GUI.hpp"
#include "SVGPathway.hpp"

int main() {
	SVGPathway::Initialize();

	std::unique_ptr<Core>& core = CreateCore(1280, 720, "NeoDoa Editor", false, "Images/neodoalogo", true);
	ImGui::SetCurrentContext(core->_window->_imGuiContext);
	/*
	ImGuiAddRenderCommand(Editor());
	*/
	std::shared_ptr<GUI> gui_ptr = std::make_shared<GUI>(core);
	ImGuiAddRenderCommand([gui = gui_ptr](float delta) { gui->operator()(delta); });

    GetCore()->Start();
    DestroyCore();

	return 0;
}