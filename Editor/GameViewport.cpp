#include <Editor/GameViewport.hpp>

#include <imgui.h>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Strings.hpp>

#include <Engine/CameraComponent.hpp>

GameViewport::GameViewport(GUI& gui) noexcept :
    gui(gui) {
    gui.Events.OnProjectLoaded   += std::bind_front(&GameViewport::OnProjectLoaded  , this);
    gui.Events.OnProjectUnloaded += std::bind_front(&GameViewport::OnProjectUnloaded, this);
    gui.Events.OnReimport        += std::bind_front(&GameViewport::OnReimport       , this);
    gui.Events.OnAssetCreated    += std::bind_front(&GameViewport::OnAssetCreated   , this);
    gui.Events.OnAssetDeleted    += std::bind_front(&GameViewport::OnAssetDeleted   , this);
}

bool GameViewport::Begin() {
    ImGui::PushID(WindowStrings::GameViewportWindowName);
    bool visible = ImGui::Begin(WindowStrings::GameViewportWindowTitleID);

    return visible;
}

void GameViewport::Render() {
    GUI& gui = this->gui;
    ImFont* font = gui.GetFontBold();

    float lineHeight = font->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    const Assets& assets = *gui.CORE->GetAssets();
    std::string_view selectedFrameBufferName{ "<<SELECT A FRAME BUFFER ASSET>>" };
    if (selectedFrameBufferID != UUID::Empty()) {
        AssetHandle handle = assets.FindAsset(selectedFrameBufferID);
        assert(handle.HasValue());
        assert(handle->IsFrameBuffer());
        selectedFrameBufferName = handle->DataAs<FrameBuffer>().Name;
    }

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Frame Buffer:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(360);
    if (ImGui::BeginCombo("##GameViewportFrameBufferComboBox", selectedFrameBufferName.data(), ImGuiComboFlags_HeightLarge)) {
        for (std::string_view currentName : frameBufferAssetNames) {
            bool is_selected = (selectedFrameBufferName == currentName);
            if (ImGui::Selectable(currentName.data(), is_selected) && !is_selected) {
                for (UUID id : assets.FrameBufferAssetIDs()) {
                    AssetHandle handle = assets.FindAsset(id);
                    assert(handle.HasValue());
                    assert(handle->IsFrameBuffer());

                    const FrameBuffer& fb = handle->DataAs<FrameBuffer>();
                    if (currentName == fb.Name) {
                        selectedFrameBufferID = id;
                        break;
                    }
                }
            }
            if (is_selected) { ImGui::SetItemDefaultFocus(); }
        }
        ImGui::EndCombo();
    }

    if (selectedFrameBufferID == UUID::Empty()) {
        selectedAttachmentIndex = 0;
        displayableAttachmentNames.clear();
    } else {
        AssetHandle handle = assets.FindAsset(selectedFrameBufferID);
        assert(handle.HasValue());
        assert(handle->IsFrameBuffer());

        FrameBuffer& fb = handle->DataAs<FrameBuffer>();
        displayableAttachmentNames.clear();

        if (0 < fb.ColorAttachments.size() && fb.ColorAttachments[0].HasReadbackSupport)            { displayableAttachmentNames.push_back(ColorAttachment0);       }
        if (1 < fb.ColorAttachments.size() && fb.ColorAttachments[1].HasReadbackSupport)            { displayableAttachmentNames.push_back(ColorAttachment1);       }
        if (2 < fb.ColorAttachments.size() && fb.ColorAttachments[2].HasReadbackSupport)            { displayableAttachmentNames.push_back(ColorAttachment2);       }
        if (3 < fb.ColorAttachments.size() && fb.ColorAttachments[3].HasReadbackSupport)            { displayableAttachmentNames.push_back(ColorAttachment3);       }
        if (4 < fb.ColorAttachments.size() && fb.ColorAttachments[4].HasReadbackSupport)            { displayableAttachmentNames.push_back(ColorAttachment4);       }
        if (5 < fb.ColorAttachments.size() && fb.ColorAttachments[5].HasReadbackSupport)            { displayableAttachmentNames.push_back(ColorAttachment5);       }
        if (6 < fb.ColorAttachments.size() && fb.ColorAttachments[6].HasReadbackSupport)            { displayableAttachmentNames.push_back(ColorAttachment6);       }
        if (7 < fb.ColorAttachments.size() && fb.ColorAttachments[7].HasReadbackSupport)            { displayableAttachmentNames.push_back(ColorAttachment7);       }
        if (fb.DepthAttachment.has_value() && fb.DepthAttachment->HasReadbackSupport)               { displayableAttachmentNames.push_back(DepthAttachment);        }
        if (fb.DepthStencilAttachment.has_value() && fb.DepthStencilAttachment->HasReadbackSupport) { displayableAttachmentNames.push_back(DepthStencilAttachment); }
    }

    if (!displayableAttachmentNames.empty()) {
        selectedAttachmentIndex = std::clamp(selectedAttachmentIndex, 0u, static_cast<unsigned>(displayableAttachmentNames.size() - 1));

        ImGui::SameLine();
        ImGui::TextUnformatted("Attachment:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(240);
        if (ImGui::BeginCombo("##GameViewportFrameBufferAttachmentComboBox", displayableAttachmentNames[selectedAttachmentIndex].data(), ImGuiComboFlags_HeightLarge)) {
            for (size_t i = 0; i < displayableAttachmentNames.size(); i++) {
                std::string_view currentName = displayableAttachmentNames[i];
                bool is_selected = (selectedAttachmentIndex == i);
                if (ImGui::Selectable(currentName.data(), is_selected)) {
                    selectedAttachmentIndex = static_cast<unsigned>(i);
                }
                if (is_selected) { ImGui::SetItemDefaultFocus(); }
            }
            ImGui::EndCombo();
        }

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 4);
        const GPUTexture& selectedAttachmentToDisplay = FetchAttachmentTexture(assets, selectedFrameBufferID, displayableAttachmentNames[selectedAttachmentIndex]);

        static float extraPadding = 4; /* pad by an extra amount to remove scroll bar, don't pad and see the scroll bar appear on right side */
        auto [availableWidth, availableHeight] = ImGui::GetContentRegionAvail();
        availableWidth = availableWidth - ImGui::GetStyle().FramePadding.x;
        availableHeight = availableHeight - extraPadding;

        float w = static_cast<float>(selectedAttachmentToDisplay.Width);
        float h = static_cast<float>(selectedAttachmentToDisplay.Height);
        float aspect = w / h;

        float maxWidth = availableWidth;
        float maxHeight = availableHeight;

        w = maxWidth;
        h = w / aspect;

        if (h > maxHeight) {
            aspect = w / h;
            h = maxHeight;
            w = h * aspect;
        }

        auto [posX, posY] = ImGui::GetCursorPos();
        if (w < maxWidth)  { posX += (maxWidth - w) / 2.0f; }
        if (h < maxHeight) { posY += (maxHeight - h) / 2.0f; }

        ImGui::SetCursorPos({ posX, posY });
        ImGui::Image(selectedAttachmentToDisplay, { w, h }, { 0, 1 }, { 1, 0 }, { 1, 1, 1, 1 }, { 0, 1, 0, 1 });
    }
}

void GameViewport::End() {
    ImGui::End();
    ImGui::PopID();
}

const GPUTexture& GameViewport::FetchAttachmentTexture(const Assets& assets, UUID frameBufferAssetID, std::string_view attachmentName) noexcept {
    const auto& gpuFrameBuffers = assets.GPUBridge().GetFrameBuffers();
    assert(gpuFrameBuffers.Exists(frameBufferAssetID));

    const GPUFrameBuffer& gpuFrameBuffer = gpuFrameBuffers.Fetch(frameBufferAssetID);

    auto extractTextureFromAttachment = [](const std::optional<std::variant<GPUTexture, GPURenderBuffer>>& attachment) -> const GPUTexture& {
        assert(attachment.has_value());
        assert(std::holds_alternative<GPUTexture>(attachment.value()));
        return std::get<GPUTexture>(attachment.value());
    };

    if (attachmentName == ColorAttachment0) {
        return extractTextureFromAttachment(gpuFrameBuffer.ColorAttachments[0]);
    } else if(attachmentName == ColorAttachment1) {
        return extractTextureFromAttachment(gpuFrameBuffer.ColorAttachments[1]);
    } else if (attachmentName == ColorAttachment2) {
        return extractTextureFromAttachment(gpuFrameBuffer.ColorAttachments[2]);
    } else if (attachmentName == ColorAttachment3) {
        return extractTextureFromAttachment(gpuFrameBuffer.ColorAttachments[3]);
    } else if (attachmentName == ColorAttachment4) {
        return extractTextureFromAttachment(gpuFrameBuffer.ColorAttachments[4]);
    } else if (attachmentName == ColorAttachment5) {
        return extractTextureFromAttachment(gpuFrameBuffer.ColorAttachments[5]);
    } else if (attachmentName == ColorAttachment6) {
        return extractTextureFromAttachment(gpuFrameBuffer.ColorAttachments[6]);
    } else if (attachmentName == ColorAttachment7) {
        return extractTextureFromAttachment(gpuFrameBuffer.ColorAttachments[7]);
    } else if (attachmentName == DepthAttachment) {
        return extractTextureFromAttachment(gpuFrameBuffer.DepthAttachment);
    } else if (attachmentName == DepthStencilAttachment) {
        return extractTextureFromAttachment(gpuFrameBuffer.DepthStencilAttachment);
    } else {
        std::unreachable();
    }
}

void GameViewport::OnProjectLoaded([[maybe_unused]] Project& project) noexcept {
    const Assets& assets = *gui.get().CORE->GetAssets();
    for (UUID id : assets.FrameBufferAssetIDs()) {
        AssetHandle handle = assets.FindAsset(id);
        assert(handle.HasValue());
        assert(handle->IsFrameBuffer());

        const FrameBuffer& fb = handle->DataAs<FrameBuffer>();
        frameBufferAssetNames.push_back(fb.Name);
    }
}
void GameViewport::OnProjectUnloaded() noexcept {
    frameBufferAssetNames.clear();
}
void GameViewport::OnReimport(Assets& assets) noexcept {
    frameBufferAssetNames.clear();
    for (UUID id : assets.FrameBufferAssetIDs()) {
        AssetHandle handle = assets.FindAsset(id);
        assert(handle.HasValue());
        assert(handle->IsFrameBuffer());

        const FrameBuffer& fb = handle->DataAs<FrameBuffer>();
        frameBufferAssetNames.push_back(fb.Name);
    }
}
void GameViewport::OnAssetCreated(AssetHandle handle) noexcept {
    assert(handle.HasValue());
    if (!handle->IsFrameBuffer()) { return; }

    const FrameBuffer& fb = handle->DataAs<FrameBuffer>();
    frameBufferAssetNames.push_back(fb.Name);
}
void GameViewport::OnAssetDeleted(AssetHandle handle) noexcept {
    assert(handle.HasValue());
    if (!handle->IsFrameBuffer()) { return; }

    const FrameBuffer& fb = handle->DataAs<FrameBuffer>();
    std::erase(frameBufferAssetNames, fb.Name);

    if(selectedFrameBufferID == handle->ID()) {
        selectedFrameBufferID = UUID::Empty();
    }
}