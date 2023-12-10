#include <Engine/Core.hpp>
#include <Engine/ImGuiRenderer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/OutlineAttachment.hpp>
#include <Editor/EditorMetaInitializer.hpp>

int main() {
    InitializeEditorMeta();

    DOA_LOG_INFO("Allocating %d bytes...", sizeof(Core));
    const CorePtr& core = Core::CreateCore({ 2000, 2000 }, "NeoDoa Editor", false, "Images/neodoalogo", true);
    DOA_LOG_INFO("Core dynamically allocated!");

    DOA_LOG_INFO("Initializing SVG Pathway @ %s", std::filesystem::current_path().string().c_str());
    ImVec4 txtColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
    SVGPathway::Initialize(std::filesystem::current_path(), { txtColor.x, txtColor.y, txtColor.z, txtColor.w });
    DOA_LOG_INFO("Initialized SVG Pathway");

    DOA_LOG_INFO("Allocating %d bytes...", sizeof(GUI));
    std::shared_ptr<GUI> gui_ptr = std::make_shared<GUI>(core);
    DOA_LOG_INFO("GUI dynamically allocated!");
    //gui_ptr->OpenProjectFromDisk("C:\\Users\\DOA\\Desktop\\test\\New Project.doa");
    ImGuiAddRenderCommand([gui = gui_ptr](float delta) { gui->operator()(delta); });

    core->CreateAttachment<OutlineAttachment>(gui_ptr);
    core->Start();
    Core::DestroyCore();

    return 0;
}