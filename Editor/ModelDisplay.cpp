#include <Editor/ModelDisplay.hpp>

#include <glm/gtx/string_cast.hpp>

#include <Engine/Core.hpp>
#include <Engine/Model.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/Observer.hpp>
#include <Editor/ImGuiExtensions.hpp>

#include <Utility/Tree.hpp>

ModelDisplay::ModelDisplay(Observer& observer) noexcept :
    observer(observer) {}

void ModelDisplay::SetDisplayTarget(const AssetHandle modelAssetHandle) noexcept {
    assert(modelAssetHandle->IsModel());
    if (modelAsset != modelAssetHandle) {
        modelAsset = modelAssetHandle;
    }
}
void ModelDisplay::RenderMessagesTable() noexcept {
    assert(modelAsset.HasValue());
    if (!modelAsset->HasErrorMessages() &&
        !modelAsset->HasWarningMessages() &&
        !modelAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Model Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (const auto& message : modelAsset->ErrorMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::ERROR_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::ERROR_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        ImGui::TextWrapped("%s", message.Message.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::WARNING_COLOR);
    for (const auto& message : modelAsset->WarningMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::WARNING_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::WARNING_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        ImGui::TextWrapped("%s", message.Message.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::INFO_COLOR);
    for (const auto& message : modelAsset->InfoMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::INFO_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::INFO_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        ImGui::TextWrapped("%s", message.Message.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::EndTable();
}
void ModelDisplay::RenderModelContents() noexcept {
    assert(modelAsset.HasValue());

    Model& model = modelAsset->DataAs<Model>();
    //DrawTreeVisualization(model.Nodes);
    if (ImGui::CollapsingHeader(std::format("{} Nodes", model.Nodes.Size()).c_str())) {
        ImGui::Indent();
        static auto recursiveRenderNodes = [](const Tree<Model::Node>& nodes, Tree<Model::Node>::NodeIndex index, std::span<const std::string> meshNames, auto&& selfReference) -> void {
            assert(nodes.HasNodeAt(index));

            const Model::Node& node{ nodes.NodeAt(index) };
            std::string title{ std::format("{}###{}", node.Name, index) };
            if (index == nodes.Root) {
                title = std::format("ROOT###{}", index);
            }

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

            const auto children = nodes.ChildrenOfNodeAt(index);
            if (index == nodes.Root) {
                flags |= ImGuiTreeNodeFlags_DefaultOpen;
            }
            if (children.size() == 0 && node.MeshIndices.size() == 0) {
                flags |= ImGuiTreeNodeFlags_Leaf;
                flags |= ImGuiTreeNodeFlags_Bullet;
            }

            if (ImGui::TreeNodeEx(title.c_str(), flags)) {
                if (node.MeshIndices.size() > 0) {
                    if (ImGui::TreeNodeEx("Mesh Index - Mesh Name", ImGuiTreeNodeFlags_DefaultOpen)) {
                        for (const auto meshIndex : node.MeshIndices) {
                            ImGui::TextUnformatted(std::format("[{}] - {}", meshIndex, meshNames[meshIndex]).c_str());
                        }
                        ImGui::TreePop();
                    }
                    ImGui::Separator();
                }

                for (size_t child : children.ChildIndices()) {
                    selfReference(nodes, child, meshNames, selfReference);
                }
                ImGui::TreePop();
            }
        };
        recursiveRenderNodes(model.Nodes, model.Nodes.Root, model.MeshNames, recursiveRenderNodes);
        ImGui::Unindent();
    }
    if (ImGui::CollapsingHeader(std::format("{} Meshes", model.Meshes.size()).c_str())) {
        ImGui::Indent();
        RenderMeshInfo(model.Meshes, model.MeshNames, model.MeshUUIDs);
        ImGui::Unindent();
    }
    if (ImGui::CollapsingHeader(std::format("{} Textures", model.Textures.size()).c_str())) {
        ImGui::Indent();
        RenderTextureInfo(model.Textures);
        ImGui::Unindent();
    }
    if (ImGui::CollapsingHeader(std::format("{} Materials", model.Materials.size()).c_str())) {
        ImGui::Indent();
        RenderMaterialInfo(model.Materials);
        ImGui::Unindent();
    }
}
void ModelDisplay::DrawTreeVisualization(const Tree<Model::Node>& tree) noexcept {
    static ImVec2 offset(250, 250); // Panning offset
    static float zoom = 1.0f; // Zoom level

    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImVec2 origin = ImGui::GetCursorScreenPos();

    // Handle zooming with mouse wheel
    if (ImGui::IsWindowHovered() && io.MouseWheel != 0.0f) {
        float zoomFactor = 1.1f;
        zoom *= (io.MouseWheel > 0) ? zoomFactor : (1.0f / zoomFactor);
        zoom = ImClamp(zoom, 0.05f, 20.0f);
    }

    // Handle panning with middle mouse button
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
        offset.x += io.MouseDelta.x;
        offset.y += io.MouseDelta.y;
    }

    // Example node positions (for now, naive layout)
    float nodeSpacing = 250.0f * zoom;
    std::vector<ImVec2> positions(tree.Size());
    positions[tree.Root] = ImVec2(origin.x + offset.x, origin.y + offset.y);
    static auto lamb = [](const Tree<Model::Node>& tree, size_t index, float nodeSpacing, std::vector<ImVec2>& positions, auto&& selfReference) -> void {
        const Tree<Model::Node>::ChildrenList children = tree.ChildrenOfNodeAt(index);

        ImVec2 parentPos = positions[index];
        float startX = parentPos.x - nodeSpacing * (children.size() / 2);

        for (size_t i = 0; i < children.size(); i++) {
            size_t idx = children.ChildIndices()[i];
            positions[idx] = ImVec2(startX + nodeSpacing * i, parentPos.y + nodeSpacing);
            selfReference(tree, idx, nodeSpacing, positions, selfReference);
        }
    };
    lamb(tree, tree.Root, nodeSpacing, positions, lamb);

    // Draw connections
    for (size_t i = 0; i < tree.Size(); i++) {
        const Tree<Model::Node>::ChildrenList children = tree.ChildrenOfNodeAt(i);
        for (size_t j = 0; j < children.size(); j++) {
            size_t childIndex = children.ChildIndices()[j];
            drawList->AddLine(positions[i], positions[childIndex], IM_COL32(200, 200, 200, 255), 2.0f);
        }
    }

    // Draw nodes
    ImGui::SetWindowFontScale(zoom);
    for (size_t i = 0; i < tree.Size(); ++i) {
        drawList->AddCircleFilled(positions[i], 15.0f * zoom, IM_COL32(100, 150, 250, 255));
        const std::string& name = tree.NodeAt(i).Name;

        // If name is too long, split it to two.
        if (name.size() > 10) {
            { // Render first half of name
                ImVec2 pos = positions[i];
                std::string firstHalf = name.substr(0, name.size() / 2);
                ImVec2 firstSize = ImGui::CalcTextSize(firstHalf.c_str());
                pos.x -= firstSize.x / 2;
                pos.y -= firstSize.y;
                drawList->AddText(pos, IM_COL32(255, 255, 255, 255), firstHalf.c_str());
            }
            { // Render second half of name
                ImVec2 pos = positions[i];
                std::string secondHalf = name.substr(name.size() / 2);
                ImVec2 secondSize = ImGui::CalcTextSize(secondHalf.c_str());
                pos.x -= secondSize.x / 2;
                drawList->AddText(pos, IM_COL32(255, 255, 255, 255), secondHalf.c_str());
            }
        } else {
            ImVec2 pos = positions[i];
            ImVec2 size = ImGui::CalcTextSize(name.c_str());
            pos.x -= size.x / 2;
            drawList->AddText(pos, IM_COL32(255, 255, 255, 255), name.c_str());
        }
    }
}

void ModelDisplay::RenderMeshInfo(std::span<const Model::Mesh> meshes, std::span<const std::string> meshNames, std::span<const UUID> meshUUIDs) noexcept {
    const static CorePtr& Core{ Core::GetCore() };
    Assets& assets{ *Core->GetAssets() };
    assert(meshes.size() == meshNames.size() && meshNames.size() == meshUUIDs.size());

    for (size_t i = 0; i < meshes.size(); i++) {
        ImGui::PushID(std::to_string(i).c_str());
        const Model::Mesh& mesh = meshes[i];
        const std::string& meshName = meshNames[i];
        const UUID& meshUUID = meshUUIDs[i];
        assert(assets.IsMeshAsset(meshUUID));

        const Mesh& meshAsset = assets.GetDataOfAssetAs<Mesh>(meshUUID);
        if (ImGui::CollapsingHeader(meshName.c_str())) {
            ImGui::Indent();
            if (ImGui::TreeNodeEx("Render Command", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::TextUnformatted("Count:");          ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", mesh.Count).c_str());
                ImGui::TextUnformatted("Base Vertex:");    ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", mesh.BaseVertex).c_str());
                ImGui::TextUnformatted("Base Index:");     ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", mesh.BaseIndex).c_str());
                ImGui::TextUnformatted("Material Index:"); ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", mesh.MaterialIndex).c_str());
                ImGui::TreePop();
            }
            ImGui::TextUnformatted("Vertex Count:"); ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", meshAsset.VertexCount).c_str());
            ImGui::TextUnformatted("Index Count:");  ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", meshAsset.IndexCount).c_str());
            if (ImGui::TreeNodeEx("AABB")) {
                ImGui::TextUnformatted("Minimum:");   ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{{ {}, {}, {} }}", meshAsset.Min.x, meshAsset.Min.y, meshAsset.Min.z).c_str());
                ImGui::TextUnformatted("Maximum:");   ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{{ {}, {}, {} }}", meshAsset.Max.x, meshAsset.Max.y, meshAsset.Max.z).c_str());
                ImGui::TextUnformatted("Origin:");    ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{{ {}, {}, {} }}", meshAsset.Origin.x, meshAsset.Origin.y, meshAsset.Origin.z).c_str());
                ImGui::TextUnformatted("Centeroid:"); ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{{ {}, {}, {} }}", meshAsset.Centeroid.x, meshAsset.Centeroid.y, meshAsset.Centeroid.z).c_str());
                ImGui::TreePop();
            }

            bool hasRAMContent = assets.AssetHasContentInSystemMemory(meshUUID);
            bool hasVRAMContent = assets.AssetHasContentInVideoMemory(meshUUID);
            ImGui::TextUnformatted("Has Data In RAM:");  ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", hasRAMContent).c_str());
            ImGui::TextUnformatted("Has Data In VRAM:"); ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", hasVRAMContent).c_str());

            float availX = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x;
            if (!hasRAMContent) {
                if (ImGui::Button("Load into system memory", { availX / 2, 0 })) {
                    assets.ReadContentOfAssetIntoSystemMemory(meshUUID);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Loads asset's data from disk into RAM. May result in high RAM usage.");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            } else {
                if (ImGui::Button("Release from system memory", { availX / 2, 0 })) {
                    assets.ReleaseContentOfAssetInSystemMemory(meshUUID);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Releases asset's data from RAM. Helps with high RAM usage.");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            }
            ImGui::SameLine();

            if (!hasVRAMContent) {
                ImGui::BeginDisabled(!hasRAMContent);
                if (ImGui::Button("Load into video memory", { availX / 2, 0 })) {
                    assets.UploadContentOfAssetIntoVideoMemory(meshUUID);
                }
                ImGui::EndDisabled();
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Loads asset's data from system memory into into VRAM. May result in high VRAM usage.");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            } else {
                if (ImGui::Button("Release from video memory", { availX / 2, 0 })) {
                    assets.ReleaseContentOfAssetInVideoMemory(meshUUID);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Releases asset's data from VRAM. Helps with high VRAM usage.");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            }
            ImGui::Unindent();
        }
        ImGui::PopID();
    }
}

void ModelDisplay::RenderTextureInfo(std::span<const Model::Texture> textures) noexcept {
    const static CorePtr& Core{ Core::GetCore() };
    Assets& assets{ *Core->GetAssets() };

    for (size_t i = 0; i < textures.size(); i++) {
        ImGui::PushID(std::to_string(i).c_str());
        const Model::Texture& texture = textures[i];
        assert(assets.IsTextureAsset(texture.TextureUUID));

        const Texture& textureData = assets.GetDataOfAssetAs<Texture>(texture.TextureUUID);
        const FNode& textureFNode = assets.GetFileOfAsset(texture.TextureUUID);
        if (ImGui::CollapsingHeader(textureData.Name.c_str())) {
            ImGui::Indent();
            ImGui::TextUnformatted("Path:");       ImGui::SameLine(200); ImGui::TextUnformatted(textureFNode.Path().string().c_str());
            ImGui::TextUnformatted("Embedded:");   ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", texture.Embedded).c_str());
            ImGui::TextUnformatted("Channels:");   ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}", textureData.Channels).c_str());
            ImGui::TextUnformatted("Dimensions:"); ImGui::SameLine(200); ImGui::TextUnformatted(std::format("{}x{}", textureData.Width, textureData.Height).c_str());
            if (assets.AssetHasContentInVideoMemory(texture.TextureUUID)) {
                const GPUTexture& gpuTexture = assets.GPUBridge().GetTextures().Fetch(texture.TextureUUID);
                float scale = 256.0f / textureData.Height;
                ImGui::Image(gpuTexture, { textureData.Width * scale, textureData.Height * scale }, { 0, 1 }, { 1, 0 });
            } else {
                ImGui::TextUnformatted("Texture has no data in video memory...");
            }

            float availX = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x;
            bool hasRAMContent = assets.AssetHasContentInSystemMemory(texture.TextureUUID);
            if (!hasRAMContent) {
                if (ImGui::Button("Load into system memory", { availX / 2, 0 })) {
                    assets.ReadContentOfAssetIntoSystemMemory(texture.TextureUUID);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Loads asset's data from disk into RAM. May result in high RAM usage.");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            } else {
                if (ImGui::Button("Release from system memory", { availX / 2, 0 })) {
                    assets.ReleaseContentOfAssetInSystemMemory(texture.TextureUUID);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Releases asset's data from RAM. Helps with high RAM usage.");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            }
            ImGui::SameLine();

            bool hasVRAMContent = assets.AssetHasContentInVideoMemory(texture.TextureUUID);
            if (!hasVRAMContent) {
                ImGui::BeginDisabled(!hasRAMContent);
                if (ImGui::Button("Load into video memory", { availX / 2, 0 })) {
                    assets.UploadContentOfAssetIntoVideoMemory(texture.TextureUUID);
                }
                ImGui::EndDisabled();
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Loads asset's data from system memory into into VRAM. May result in high VRAM usage.");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            } else {
                if (ImGui::Button("Release from video memory", { availX / 2, 0 })) {
                    assets.ReleaseContentOfAssetInVideoMemory(texture.TextureUUID);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Releases asset's data from VRAM. Helps with high VRAM usage.");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            }
            ImGui::Unindent();
        }

        ImGui::PopID();
    }
}

void ModelDisplay::RenderMaterialInfo(std::span<const Model::Material> materials) noexcept {
    for (size_t i = 0; i < materials.size(); i++) {
        ImGui::PushID(std::to_string(i).c_str());

        const Model::Material& material = materials[i];
        std::string name = material.Name.value_or("(missing_name)");

        if (ImGui::CollapsingHeader(name.c_str())) {
            ImGui::Indent();
            RenderMaterialColorInfo("Diffuse Color",       material.DiffuseColor);
            RenderMaterialColorInfo("Specular Color",      material.SpecularColor);
            RenderMaterialColorInfo("Ambient Color",       material.AmbientColor);
            RenderMaterialColorInfo("Emissive Color",      material.EmissiveColor);
            RenderMaterialColorInfo("Transparent Color",   material.TransparentColor);
            RenderMaterialColorInfo("Reflective Color",    material.ReflectiveColor);
            RenderMaterialFloatInfo("Reflectivity",        material.Reflectivity);
            RenderMaterialBoolInfo("Enable Wireframe",     material.EnableWireframe);
            RenderMaterialBoolInfo("Two Sided",            material.TwoSided);
            RenderMaterialShadingModeInfo("Shading Mode",  material.Shading);
            RenderMaterialBlendModeInfo("Blend Mode",      material.Blend);
            RenderMaterialFloatInfo("Opacity",             material.Opacity);
            RenderMaterialFloatInfo("Shininess",           material.Shininess);
            RenderMaterialFloatInfo("Shininess Strength",  material.ShininessStrength);
            RenderMaterialFloatInfo("Refracti",            material.Refracti);

            if (ImGui::TreeNodeEx("Texture Stacks")) {
                for (size_t j = 0; j < material.TextureStacks.size(); j++) {
                    const Model::Material::TextureStack& stack = material.TextureStacks[j];
                    if (stack.TextureCount == 0) { continue; }

                    Model::Material::TextureType stackType = static_cast<Model::Material::TextureType>(j);

                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
                    if (ImGui::TreeNodeEx(ToString(stackType).data(), flags)) {
                        RenderMaterialTextureStack(stack);
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
            ImGui::Unindent();
        }

        ImGui::PopID();
    }
}
void ModelDisplay::RenderMaterialColorInfo(std::string_view name, const std::optional<Color>& color) noexcept {
    ImGui::PushID(name.data());

    ImGui::TextUnformatted(std::format("{}:", name).c_str());
    ImGui::SameLine(200);
    if (color.has_value()) {
        const Color& c = color.value();
        ImGui::ColorButton(name.data(), c.ToImVec4(), ImGuiColorEditFlags_AlphaPreviewHalf, { ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() });
        ImGui::SameLine();
        ImGui::TextUnformatted(c.ToString().c_str());
    } else {
        ImGui::BeginDisabled();
        ImGui::TextUnformatted("Missing Value");
        ImGui::EndDisabled();
    }

    ImGui::PopID();
}
void ModelDisplay::RenderMaterialFloatInfo(std::string_view name, const std::optional<float>& floatValue) noexcept {
    ImGui::PushID(name.data());

    ImGui::TextUnformatted(std::format("{}:", name).c_str());
    ImGui::SameLine(200);
    if (floatValue.has_value()) {
        ImGui::TextUnformatted(std::format("{}", floatValue.value()).c_str());
    } else {
        ImGui::BeginDisabled();
        ImGui::TextUnformatted("Missing Value");
        ImGui::EndDisabled();
    }

    ImGui::PopID();
}
void ModelDisplay::RenderMaterialBoolInfo(std::string_view name, const std::optional<bool>& boolValue) noexcept {
    ImGui::PushID(name.data());

    ImGui::TextUnformatted(std::format("{}:", name).c_str());
    ImGui::SameLine(200);
    if (boolValue.has_value()) {
        ImGui::TextUnformatted(std::format("{}", boolValue.value()).c_str());
    } else {
        ImGui::BeginDisabled();
        ImGui::TextUnformatted("Missing Value");
        ImGui::EndDisabled();
    }

    ImGui::PopID();
}
void ModelDisplay::RenderMaterialShadingModeInfo(std::string_view name, const std::optional<Model::Material::ShadingMode>& shading) noexcept {
    ImGui::PushID(name.data());

    ImGui::TextUnformatted(std::format("{}:", name).c_str());
    ImGui::SameLine(200);
    if (shading.has_value()) {
        ImGui::TextUnformatted(ToString(shading.value()).data());
    } else {
        ImGui::BeginDisabled();
        ImGui::TextUnformatted("Missing Value");
        ImGui::EndDisabled();
    }

    ImGui::PopID();
}
void ModelDisplay::RenderMaterialBlendModeInfo(std::string_view name, const std::optional<Model::Material::BlendMode>& blend) noexcept {
    ImGui::PushID(name.data());

    ImGui::TextUnformatted(std::format("{}:", name).c_str());
    ImGui::SameLine(200);
    if (blend.has_value()) {
        ImGui::TextUnformatted(ToString(blend.value()).data());
    } else {
        ImGui::BeginDisabled();
        ImGui::TextUnformatted("Missing Value");
        ImGui::EndDisabled();
    }

    ImGui::PopID();
}
void ModelDisplay::RenderMaterialTextureStack(const Model::Material::TextureStack& stack) noexcept {
    if (stack.TextureCount == 1) {
        RenderMaterialTextureInfo(stack.Textures[0]);
    } else {
        for (size_t i = 0; i < stack.TextureCount; i++) {
            const Model::Material::TextureInfo& info = stack.Textures[i];

            if (ImGui::TreeNodeEx(std::format("Texture[{}]", i).c_str())) {
                RenderMaterialTextureInfo(info);
                ImGui::TreePop();
            }
        }
    }
}
void ModelDisplay::RenderMaterialTextureInfo(const Model::Material::TextureInfo& info) noexcept {
    ImGui::TextUnformatted("Relative Path:");  ImGui::SameLine(200); ImGui::TextUnformatted(info.Path.c_str());
    ImGui::TextUnformatted("BlendAmount:");    ImGui::SameLine(200); ImGui::TextUnformatted(std::to_string(info.BlendAmount).c_str());
    ImGui::TextUnformatted("Operation:");      ImGui::SameLine(200); ImGui::TextUnformatted(ToString(info.Operation).data());
    ImGui::TextUnformatted("Mapping:");        ImGui::SameLine(200); ImGui::TextUnformatted(ToString(info.Mapping).data());
    ImGui::TextUnformatted("UVWSource:");      ImGui::SameLine(200); ImGui::TextUnformatted(std::to_string(info.UVWSource).data());
    ImGui::TextUnformatted("MapModeU:");       ImGui::SameLine(200); ImGui::TextUnformatted(ToString(info.MapModeU).data());
    ImGui::TextUnformatted("MapModeV:");       ImGui::SameLine(200); ImGui::TextUnformatted(ToString(info.MapModeV).data());
    ImGui::TextUnformatted("MappingAxis:");    ImGui::SameLine(200); ImGui::TextUnformatted(glm::to_string(info.MappingAxis).c_str());
    ImGui::TextUnformatted("Flags:");          ImGui::SameLine(200); ImGui::TextUnformatted(ToString(info.Flags).c_str());
}

void ModelDisplay::OnAssetRefreshed(AssetHandle handle) noexcept {
    if (modelAsset == handle) {
        assert(handle->IsModel());

        modelAsset = {}; // SetDisplayTarget returns if meshAsset == handle. Force it to do work.
        SetDisplayTarget(handle);
    }
}
