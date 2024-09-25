#include <Editor/FrameBufferDisplay.hpp>

#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/ImGuiExtensions.hpp>
#include <Editor/ComponentWidgets.hpp>

#include <Utility/NameOf.hpp>
#include <Utility/Prettify.hpp>

FrameBufferDisplay::FrameBufferDisplay(Observer& observer) noexcept :
    observer(observer) {
    using enum DataFormat;
    colorFormatEnums.emplace_back("", static_cast<int>(R8          ), nameof(R8          ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG8         ), nameof(RG8         ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB8        ), nameof(RGB8        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA8       ), nameof(RGBA8       ));
    colorFormatEnums.emplace_back("", static_cast<int>(R16         ), nameof(R16         ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG16        ), nameof(RG16        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB16       ), nameof(RGB16       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA16      ), nameof(RGBA16      ));
    colorFormatEnums.emplace_back("", static_cast<int>(R16F        ), nameof(R16F        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG16F       ), nameof(RG16F       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB16F      ), nameof(RGB16F      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA16F     ), nameof(RGBA16F     ));
    colorFormatEnums.emplace_back("", static_cast<int>(R32F        ), nameof(R32F        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG32F       ), nameof(RG32F       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB32F      ), nameof(RGB32F      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA32F     ), nameof(RGBA32F     ));
    colorFormatEnums.emplace_back("", static_cast<int>(R8_SNORM    ), nameof(R8_SNORM    ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG8_SNORM   ), nameof(RG8_SNORM   ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB8_SNORM  ), nameof(RGB8_SNORM  ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA8_SNORM ), nameof(RGBA8_SNORM ));
    colorFormatEnums.emplace_back("", static_cast<int>(R16_SNORM   ), nameof(R16_SNORM   ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG16_SNORM  ), nameof(RG16_SNORM  ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB16_SNORM ), nameof(RGB16_SNORM ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA16_SNORM), nameof(RGBA16_SNORM));
    colorFormatEnums.emplace_back("", static_cast<int>(R8UI        ), nameof(R8UI        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG8UI       ), nameof(RG8UI       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB8UI      ), nameof(RGB8UI      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA8UI     ), nameof(RGBA8UI     ));
    colorFormatEnums.emplace_back("", static_cast<int>(R16UI       ), nameof(R16UI       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG16UI      ), nameof(RG16UI      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB16UI     ), nameof(RGB16UI     ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA16UI    ), nameof(RGBA16UI    ));
    colorFormatEnums.emplace_back("", static_cast<int>(R32UI       ), nameof(R32UI       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG32UI      ), nameof(RG32UI      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB32UI     ), nameof(RGB32UI     ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA32UI    ), nameof(RGBA32UI    ));
    colorFormatEnums.emplace_back("", static_cast<int>(R8I         ), nameof(R8I         ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG8I        ), nameof(RG8I        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB8I       ), nameof(RGB8I       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA8I      ), nameof(RGBA8I      ));
    colorFormatEnums.emplace_back("", static_cast<int>(R16I        ), nameof(R16I        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG16I       ), nameof(RG16I       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB16I      ), nameof(RGB16I      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA16I     ), nameof(RGBA16I     ));
    colorFormatEnums.emplace_back("", static_cast<int>(R32I        ), nameof(R32I        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RG32I       ), nameof(RG32I       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB32I      ), nameof(RGB32I      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA32I     ), nameof(RGBA32I     ));
    colorFormatEnums.emplace_back("", static_cast<int>(SRGB8       ), nameof(SRGB8       ));
    colorFormatEnums.emplace_back("", static_cast<int>(SRGBA8      ), nameof(SRGBA8      ));
    colorFormatEnums.emplace_back("", static_cast<int>(R3G3B2      ), nameof(R3G3B2      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB5A1      ), nameof(RGB5A1      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB10A2     ), nameof(RGB10A2     ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB10A2UI   ), nameof(RGB10A2UI   ));
    colorFormatEnums.emplace_back("", static_cast<int>(R11FG11FB10F), nameof(R11FG11FB10F));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB9E5      ), nameof(RGB9E5      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB4        ), nameof(RGB4        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB5        ), nameof(RGB5        ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB565      ), nameof(RGB565      ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB10       ), nameof(RGB10       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGB12       ), nameof(RGB12       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA2       ), nameof(RGBA2       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA4       ), nameof(RGBA4       ));
    colorFormatEnums.emplace_back("", static_cast<int>(RGBA12      ), nameof(RGBA12      ));

    depthFormatEnums.emplace_back("", static_cast<int>(DEPTH16 ), nameof(DEPTH16 ));
    depthFormatEnums.emplace_back("", static_cast<int>(DEPTH24 ), nameof(DEPTH24 ));
    depthFormatEnums.emplace_back("", static_cast<int>(DEPTH32 ), nameof(DEPTH32 ));
    depthFormatEnums.emplace_back("", static_cast<int>(DEPTH32F), nameof(DEPTH32F));

    stencilFormatEnums.emplace_back("", static_cast<int>(STENCIL1 ), nameof(STENCIL1 ));
    stencilFormatEnums.emplace_back("", static_cast<int>(STENCIL4 ), nameof(STENCIL4 ));
    stencilFormatEnums.emplace_back("", static_cast<int>(STENCIL8 ), nameof(STENCIL8 ));
    stencilFormatEnums.emplace_back("", static_cast<int>(STENCIL16), nameof(STENCIL16));

    depthStencilFormatEnums.emplace_back("", static_cast<int>(DEPTH24_STENCIL8 ), nameof(DEPTH24_STENCIL8 ));
    depthStencilFormatEnums.emplace_back("", static_cast<int>(DEPTH32F_STENCIL8), nameof(DEPTH32F_STENCIL8));

    using enum Multisample;
    multisampleEnums.emplace_back("", static_cast<int>(None), nameof(None));
    multisampleEnums.emplace_back("", static_cast<int>(x2  ), nameof(x2  ));
    multisampleEnums.emplace_back("", static_cast<int>(x4  ), nameof(x4  ));
    multisampleEnums.emplace_back("", static_cast<int>(x8  ), nameof(x8  ));
}

void FrameBufferDisplay::SetDisplayTarget(const AssetHandle frameBufferAssetHandle) noexcept {
    assert(frameBufferAssetHandle->IsFrameBuffer());
    if (frameBufferAsset != frameBufferAssetHandle) {
        frameBufferAsset = frameBufferAssetHandle;
    }
}
void FrameBufferDisplay::RenderMessagesTable() noexcept {
    assert(frameBufferAsset.HasValue());
    if (!frameBufferAsset->HasErrorMessages() &&
        !frameBufferAsset->HasWarningMessages() &&
        !frameBufferAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("compiler_logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Frame Buffer Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : frameBufferAsset->ErrorMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::ERROR_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::ERROR_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const std::string& m{ std::any_cast<const std::string&>(message) };
        ImGui::TextWrapped("%s", m.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::WARNING_COLOR);
    for (auto& message : frameBufferAsset->WarningMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::WARNING_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::WARNING_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const std::string& m{ std::any_cast<const std::string&>(message) };
        ImGui::TextWrapped("%s", m.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::INFO_COLOR);
    for (auto& message : frameBufferAsset->InfoMessages()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        float r = BeginTableColumnCenterText(ComponentDefinitionViewIcons::INFO_ICON);
        ImGui::Text(ComponentDefinitionViewIcons::INFO_ICON);
        EndTableColumnCenterText(r);

        ImGui::TableSetColumnIndex(1);

        const std::string& m{ std::any_cast<const std::string&>(message) };
        ImGui::TextWrapped("%s", m.c_str());
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::EndTable();
}
void FrameBufferDisplay::RenderColorAttachments() noexcept {
    bool changed{ false };

    ImGui::PushID(nameof_c(FrameBuffer::ColorAttachments));

    ImGui::SeparatorText(Prettify(nameof(FrameBuffer::ColorAttachments)).c_str());
    auto& colorAttachments = frameBufferAsset->DataAs<FrameBuffer>().ColorAttachments;
    ImGui::Indent();
    for (size_t i = 0; i < colorAttachments.size(); i++) {
        auto id = std::format("[{}]", i);
        ImGui::PushID(id.c_str());
        ImGui::SeparatorText(id.c_str());

        changed |= RenderAttachment(colorAttachments[i], colorFormatEnums);
        ImGui::PopID();
    }
    ImGui::Unindent();

    auto result = RenderAddRemoveButtons(0, static_cast<unsigned>(colorAttachments.size()), MaxFrameBufferColorAttachments);

    if (result == FrameBufferDisplay::AddRemoveResult::Add) {
        changed = true;
        colorAttachments.emplace_back();
    } else if (result == FrameBufferDisplay::AddRemoveResult::Remove) {
        changed = true;
        colorAttachments.pop_back();
    }

    ImGui::PopID();

    if (changed) {
        frameBufferAsset->Serialize();
        frameBufferAsset->ForceDeserialize();
    }
}
void FrameBufferDisplay::RenderDepthAttachment() noexcept {
    bool changed{ false };

    ImGui::PushID(nameof_c(FrameBuffer::DepthAttachment));

    ImGui::SeparatorText(Prettify(nameof(FrameBuffer::DepthAttachment)).c_str());
    auto& depthAttachment = frameBufferAsset->DataAs<FrameBuffer>().DepthAttachment;

    if(depthAttachment.has_value()) {
        changed = RenderAttachment(depthAttachment.value(), depthFormatEnums);
    }

    unsigned elementCount = static_cast<unsigned>(depthAttachment.has_value());
    auto result = RenderAddRemoveButtons(0, elementCount, 1);

    if (result == FrameBufferDisplay::AddRemoveResult::Add) {
        changed = true;
        auto& depthStencilAttachment = frameBufferAsset->DataAs<FrameBuffer>().DepthStencilAttachment;

        depthStencilAttachment.reset();
        depthAttachment.emplace();
        depthAttachment->Format = DataFormat::DEPTH32;
    } else if (result == FrameBufferDisplay::AddRemoveResult::Remove) {
        changed = true;
        depthAttachment.reset();
    }

    ImGui::PopID();

    if (changed) {
        frameBufferAsset->Serialize();
        frameBufferAsset->ForceDeserialize();
    }
}
void FrameBufferDisplay::RenderStencilAttachment() noexcept {
    bool changed{ false };

    ImGui::PushID(nameof_c(FrameBuffer::StencilAttachment));

    ImGui::SeparatorText(Prettify(nameof(FrameBuffer::StencilAttachment)).c_str());
    auto& stencilAttachment = frameBufferAsset->DataAs<FrameBuffer>().StencilAttachment;

    if (stencilAttachment.has_value()) {
        changed = RenderAttachment(stencilAttachment.value(), stencilFormatEnums);
    }

    unsigned elementCount = static_cast<unsigned>(stencilAttachment.has_value());
    auto result = RenderAddRemoveButtons(0, elementCount, 1);

    if (result == FrameBufferDisplay::AddRemoveResult::Add) {
        changed = true;
        auto& depthStencilAttachment = frameBufferAsset->DataAs<FrameBuffer>().DepthStencilAttachment;

        depthStencilAttachment.reset();
        stencilAttachment.emplace();
        stencilAttachment->Format = DataFormat::STENCIL8;
    } else if (result == FrameBufferDisplay::AddRemoveResult::Remove) {
        changed = true;
        stencilAttachment.reset();
    }

    ImGui::PopID();

    if (changed) {
        frameBufferAsset->Serialize();
        frameBufferAsset->ForceDeserialize();
    }
}
void FrameBufferDisplay::RenderDepthStencilAttachment() noexcept {
    bool changed{ false };

    ImGui::PushID(nameof_c(FrameBuffer::DepthStencilAttachment));

    ImGui::SeparatorText(Prettify(nameof(FrameBuffer::DepthStencilAttachment)).c_str());
    auto& depthStencilAttachment = frameBufferAsset->DataAs<FrameBuffer>().DepthStencilAttachment;

    if (depthStencilAttachment.has_value()) {
        changed = RenderAttachment(depthStencilAttachment.value(), depthStencilFormatEnums);
    }

    unsigned elementCount = static_cast<unsigned>(depthStencilAttachment.has_value());
    auto result = RenderAddRemoveButtons(0, elementCount, 1);

    if (result == FrameBufferDisplay::AddRemoveResult::Add) {
        changed = true;
        auto& depthAttachment = frameBufferAsset->DataAs<FrameBuffer>().DepthAttachment;
        auto& stencilAttachment = frameBufferAsset->DataAs<FrameBuffer>().StencilAttachment;

        depthAttachment.reset();
        stencilAttachment.reset();
        depthStencilAttachment.emplace();
        depthStencilAttachment->Format = DataFormat::DEPTH24_STENCIL8;
    } else if (result == FrameBufferDisplay::AddRemoveResult::Remove) {
        changed = true;
        depthStencilAttachment.reset();
    }

    ImGui::PopID();

    if (changed) {
        frameBufferAsset->Serialize();
        frameBufferAsset->ForceDeserialize();
    }
}

bool FrameBufferDisplay::RenderAttachment(FrameBufferAttachment& attachment, const std::vector<EnumValue>& formats) noexcept {
    bool rv{ false };

    rv |= ResolutionWidget(nameof(FrameBufferAttachment::Resolution), attachment.Resolution);

    int format = static_cast<int>(attachment.Format);
    rv |= EnumWidget(nameof(FrameBufferAttachment::Format), format, formats);
    attachment.Format = static_cast<DataFormat>(format);

    int samples = static_cast<int>(attachment.Samples);
    rv |= EnumWidget(nameof(FrameBufferAttachment::Samples), samples, multisampleEnums);
    attachment.Samples = static_cast<Multisample>(samples);

    rv |= BoolWidget(Prettify(nameof(FrameBufferAttachment::HasReadbackSupport)), attachment.HasReadbackSupport);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted("If this attachment will be read from CPU, or be sampled in a shader - enable this option. Drivers may optimize the process of rendering when programmers correctly set this flag. Disabling this option will render it invisible in the \"Game Viewport\" window.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    return rv;
}

FrameBufferDisplay::AddRemoveResult FrameBufferDisplay::RenderAddRemoveButtons(unsigned minElements, unsigned currentElements, unsigned maxElements) noexcept {
    AddRemoveResult rv { FrameBufferDisplay::AddRemoveResult::None };

    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[1];
    float lineHeight = boldFont->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight, lineHeight };
    float margin = 3.0f;

    BeginWidget("");
    float x = ImGui::GetCursorPosX() + compFieldWidth - 2 * (buttonSize.x) - margin;
    ImGui::SetCursorPosX(x);
    bool isEmpty = currentElements == minElements;
    bool isFull = currentElements == maxElements;
    ImGui::BeginDisabled(isEmpty);
    if (ImGui::Button(ICON_FA_MINUS, buttonSize)) {
        rv = FrameBufferDisplay::AddRemoveResult::Remove;
    }
    ImGui::EndDisabled();
    x += margin + buttonSize.x;

    ImGui::SameLine();
    ImGui::SetCursorPosX(x);
    ImGui::BeginDisabled(isFull);
    if (ImGui::Button(ICON_FA_PLUS_LARGE, buttonSize)) {
        rv = FrameBufferDisplay::AddRemoveResult::Add;
    }
    ImGui::EndDisabled();
    x += margin + buttonSize.x;
    EndWidget();

    return rv;
}