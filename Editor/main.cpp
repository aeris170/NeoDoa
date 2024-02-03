#include <argparse/argparse.hpp>

#include <Engine/Core.hpp>
#include <Engine/ImGuiRenderer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/OutlineAttachment.hpp>

int main(int argc, char* argv[]) {
    DOA_LOG_TRACE("NeoDoa Editor");
    DOA_LOG_TRACE("//- CMD Arguments -//");
    for (int i = 0; i < argc; i++) {
        DOA_LOG_TRACE("\t%s", argv[i]);
    }
    DOA_LOG_TRACE("//- CMD Arguments -//");

    //- Parse Command Line Arguments -//
    argparse::ArgumentParser program(argv[0]);
    program.add_argument("project_path").help("Absolute path to project file *.doa").metavar("PROJECT_PATH");
    program.add_description("Launches NeoDoa Editor with a loaded project.");

    try {
        program.parse_args(argc, argv);   // Example: ./main --input_files config.yml System.xml
    } catch (const std::exception& err) {
        DOA_LOG_FATAL("FATAL ERROR: %s\n", err.what());
        std::cerr << program << std::endl;
        std::exit(1);
    }
    std::string path = program.get("project_path");
    //- Parse Command Line Arguments -//

    DOA_LOG_INFO("Allocating %d bytes...", sizeof(Core));
    const CorePtr& core = Core::CreateCore({ 2000, 2000 }, "NeoDoa Editor", false, "Images/neodoalogo", true);
    core->Window()->Maximize();
    ImGui::GetIO().IniFilename = NULL;
    DOA_LOG_INFO("Core dynamically allocated!");

    DOA_LOG_INFO("Initializing SVG Pathway @ %s", std::filesystem::current_path().string().c_str());
    ImVec4 txtColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
    SVGPathway::Initialize(std::filesystem::current_path(), { txtColor.x, txtColor.y, txtColor.z, txtColor.w });
    DOA_LOG_INFO("Initialized SVG Pathway");

    DOA_LOG_INFO("Allocating %d bytes...", sizeof(GUI));
    std::shared_ptr<GUI> gui_ptr = std::make_shared<GUI>(core);
    DOA_LOG_INFO("GUI dynamically allocated!");
    //gui_ptr->OpenProjectFromDisk("C:\\Users\\DOA\\Desktop\\test\\New Project.doa");
    gui_ptr->OpenProjectFromDisk(path);
    ImGuiAddRenderCommand([gui = gui_ptr](float delta) { gui->operator()(delta); });

    core->CreateAttachment<OutlineAttachment>(gui_ptr);
    core->Start();
    Core::DestroyCore();

    return 0;
}