#include <Core.hpp>,
#include <ImGuiRenderer.hpp>

#include "GUI.hpp"
#include "OutlineAttachment.hpp"

#include <iostream>
int main() {
	std::cout << entt::type_id<float> << " " << entt::type_id<float*>;

	CorePtr& core = Core::CreateCore({ 2000, 2000 }, "NeoDoa Editor", false, "Images/neodoalogo", true);

	std::shared_ptr<GUI> gui_ptr = std::make_shared<GUI>(core);
	ImGuiAddRenderCommand([gui = gui_ptr](float delta) { gui->operator()(delta); });

	core->CreateAttachment<OutlineAttachment>(gui_ptr);
	core->Start();
	Core::DestroyCore();

	return 0;
}