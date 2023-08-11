#include "OutlineAttachment.hpp"

OutlineAttachment::OutlineAttachment(std::shared_ptr<GUI> gui) noexcept :
    gui(gui),
    fbo() {}

void OutlineAttachment::BeforeFrame(Project* project) {}
void OutlineAttachment::AfterFrame(Project* project) {
    if (project == nullptr || !project->HasOpenScene()) { return; }
    if (gui->sh.selectedEntity == NULL_ENTT) { return; }

    Scene& scene = project->GetOpenScene();
    Entity entt = gui->sh.selectedEntity;
    //if (!scene.HasComponent<MeshComponent>(entt)) return;
    TransformComponent& transformComponent = scene.GetComponent<TransformComponent>(entt);
    //MeshComponent& meshComponent = scene.GetComponent<MeshComponent>(entt);

    fbo.Bind();
    // Graphics::DrawMesh(meshComponent.mesh, transformComponent.GetWorldMatrix(), solidColorShader, )
    glDepthMask(GL_FALSE);
}