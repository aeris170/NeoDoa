#include <argparse/argparse.hpp>

#include <stb_image.h>

#include <Engine/Core.hpp>
#include <Engine/ImGuiRenderer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/OutlineAttachment.hpp>

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

    std::string path;
    try {
        program.parse_args(argc, argv);
        path = program.get("project_path");
    } catch (const std::exception& err) {
        DOA_LOG_FATAL("FATAL ERROR: %s\n", err.what());
        path = "C:\\Users\\doga.oruc\\Desktop\\Repo\\.rec\\Test\\T.doa";
        //std::cerr << program << std::endl;
        //std::exit(1);
    }
    //- Parse Command Line Arguments -//

    //- Setup params -//
    WindowIconPack pack{};
    pack.Icons.emplace_back(LoadIcon("Images/neodoalogo-16_x_16.png"));
    pack.Icons.emplace_back(LoadIcon("Images/neodoalogo-32_x_32.png"));
    pack.Icons.emplace_back(LoadIcon("Images/neodoalogo-48_x_48.png"));
    pack.Icons.emplace_back(LoadIcon("Images/neodoalogo-64_x_64.png"));
    pack.Icons.emplace_back(LoadIcon("Images/neodoalogo-128_x_128.png"));
    pack.Icons.emplace_back(LoadIcon("Images/neodoalogo-256_x_256.png"));

    ContextWindowCreationParams params {
        .Size{ 2000, 2000 },
        .Title{ "NeoDoa Editor" },

        .IsResizable{ true },
        .IsVisible{ true },
        .IsMaximized{ true },

        .Samples{ 8 },

        .IconPack{ pack },
    };
    //- Setup params -//

    // TODO change these to trace
    DOA_LOG_INFO("Allocating %d bytes...", sizeof(Core));
    const CorePtr& core = Core::CreateCore(GraphicsBackend::OpenGL4_6, WindowBackend::GLFW, params);
    ImGui::GetIO().IniFilename = NULL;
    DOA_LOG_INFO("Core dynamically allocated!");

    DOA_LOG_INFO("Initializing SVG Pathway @ %s", std::filesystem::current_path().string().c_str());
    ImVec4 txtColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
    SVGPathway::Initialize(std::filesystem::current_path(), { txtColor.x, txtColor.y, txtColor.z, txtColor.w });
    DOA_LOG_INFO("Initialized SVG Pathway");

    DOA_LOG_INFO("Allocating %d bytes...", sizeof(GUI));
    std::shared_ptr<GUI> gui_ptr = std::make_shared<GUI>(core);
    DOA_LOG_INFO("GUI dynamically allocated!");
    gui_ptr->OpenProjectFromDisk(path);
    ImGuiAddRenderCommand([gui = gui_ptr](float delta) { gui->operator()(delta); });

    core->CreateAttachment<OutlineAttachment>(gui_ptr);
    core->Start();
    Core::DestroyCore();

    gui_ptr.reset();

    return 0;
}