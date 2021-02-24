#include "NeoDoa.hpp"

#include "Editor.hpp"

int main() {
	std::unique_ptr<Core>& core = CreateCore(1280, 720, "NeoDoa Editor", false, "Images/neodoalogo", true);
	ImGui::SetCurrentContext(core->_window->_imGuiContext);
	ImGuiAddRenderCommand(Editor());

    std::vector<Vertex> vertices;
    vertices.emplace_back(Vertex{ { -1, 1, 0 }, { 0, 0, 1 }, { 1, 0.5f, 0.2f, 1 }, { 0, 1 }, -1 });
    vertices.emplace_back(Vertex{ { 1, 1, 0 }, { 0, 0, 1 }, { 1, 0.5f, 0.2f, 1 }, { 1, 1 }, -1 });
    vertices.emplace_back(Vertex{ { 1, -1, 0 }, { 0, 0, 1 }, { 1, 0.5f, 0.2f, 1 }, { 1, 0 }, -1 });
    vertices.emplace_back(Vertex{ { -1, -1, 0 }, { 0, 0, 1 }, { 1, 0.5f, 0.2f, 1 }, { 0, 0 }, -1 });
    srand((unsigned)time(NULL)); //TODO remove
    for (auto& v : vertices) {
        glm::vec4 n = { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1 };
        v.color = n;
    }

    std::vector<GLuint> indices;
    indices.emplace_back(0);
    indices.emplace_back(3);
    indices.emplace_back(2);
    indices.emplace_back(0);
    indices.emplace_back(2);
    indices.emplace_back(1);

    std::vector<std::weak_ptr<Texture>> textures;
    textures.emplace_back(CreateTexture("tex1", "Images/ch.jpg").value());

    Mesh data(std::move(vertices), std::move(indices), std::move(textures));
    std::vector<Mesh> a;
    a.emplace_back(std::move(data));

    std::weak_ptr<Model> model = CreateModelFromFileSystem("Models/12140_Skull_v3_L2.obj").value();
    std::weak_ptr<Model> model2 = CreateModelFromFileSystem("Models/Christmas tree N260410.3ds").value();
    std::weak_ptr<Model> model3 = CreateModelFromMesh("quad", a);

    std::weak_ptr<Scene> scene = CreateScene("Simge'yi coooooooook seviyorummm :)");
    auto ptr = scene.lock();
    ChangeScene(scene);

    EntityID o1 = ptr->CreateEntity("Boi");
    ScriptComponent& o1m = ptr->Modules(o1);
    Transform& tr = o1m["Transform"].As<Transform>();
    tr.Translation() = { -16, -27.4, -1.4 };
    tr.Rotation() = glm::quat({ glm::radians(-115.1), glm::radians(23.8), 0 });
    tr.Scale() = { .5f, .5f, .5f };
    {
        ModelRenderer& mr = o1m.Attach("ModelRenderer").As<ModelRenderer>();
        mr.Model() = &*model.lock();
        mr.Shader() = &*FindShader("Simple Shader").value().lock();
    }

    EntityID o2 = ptr->CreateEntity("Boi Vol. 2");
    ScriptComponent& o2m = ptr->Modules(o2);
    Transform& tr2 = o2m["Transform"].As<Transform>();
    tr2.Translation() = { 16, -27.4, -1.4 };
    tr2.Rotation() = glm::quat({ glm::radians(-115.1), glm::radians(-23.8), 0 });
    tr2.Scale() = { .5f, .5f, .5f };
    {
        ModelRenderer& mr = o2m.Attach("ModelRenderer").As<ModelRenderer>();
        mr.Model() = &*model.lock();
        mr.Shader() = &*FindShader("Simple Shader").value().lock();
    }

    EntityID o3 = ptr->CreateEntity("Tree");
    ScriptComponent& o3m = ptr->Modules(o3);
    Transform& tr3 = o3m["Transform"].As<Transform>();
    tr3.Translation() = { 0, -35, -35 };
    tr3.Scale() = { .02f, .02f, .02f };
    {
        ModelRenderer& mr = o3m.Attach("ModelRenderer").As<ModelRenderer>();
        mr.Model() = &*model2.lock();
        mr.Shader() = &*FindShader("Simple Shader").value().lock();
    }
    o3m.Attach("Rotate");

    ptr->SetPerpectiveCamera(110, 16.f / 9, 0.001, 1000);
    ptr->_pc.position.z = 45;
    ptr->_pc.zoom = 2;

    GetCore()->ClearColor = glm::vec3{ 26 / 255.f, 3 / 255.f, 30 / 255.f };
    GetCore()->Start();
	return 0;
}