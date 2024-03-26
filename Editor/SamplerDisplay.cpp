#include <Editor/SamplerDisplay.hpp>

#include <Utility/NameOf.hpp>
#include <Utility/Prettify.hpp>

#include <Engine/Sampler.hpp>

#include <Editor/Icons.hpp>
#include <Editor/Colors.hpp>
#include <Editor/Observer.hpp>
#include <Editor/ImGuiExtensions.hpp>
#include <Editor/ComponentWidgets.hpp>

SamplerDisplay::SamplerDisplay(Observer& observer) noexcept :
    observer(observer) {
    minFilterEnums.emplace_back("", 0, Prettify(nameof(TextureMinificationMode::Nearest)));
    minFilterEnums.emplace_back("", 1, Prettify(nameof(TextureMinificationMode::Linear)));
    minFilterEnums.emplace_back("", 2, Prettify(nameof(TextureMinificationMode::NearestMipmapNearest)));
    minFilterEnums.emplace_back("", 3, Prettify(nameof(TextureMinificationMode::LinearMipmapNearest)));
    minFilterEnums.emplace_back("", 4, Prettify(nameof(TextureMinificationMode::NearestMipmapLinear)));
    minFilterEnums.emplace_back("", 5, Prettify(nameof(TextureMinificationMode::LinearMipmapLinear)));

    magFilterEnums.emplace_back("", 0, Prettify(nameof(TextureMagnificationMode::Nearest)));
    magFilterEnums.emplace_back("", 1, Prettify(nameof(TextureMagnificationMode::Linear)));

    wrapModeEnums.emplace_back("", 0, Prettify(nameof(TextureWrappingMode::Repeat)));
    wrapModeEnums.emplace_back("", 1, Prettify(nameof(TextureWrappingMode::MirroredRepeat)));
    wrapModeEnums.emplace_back("", 2, Prettify(nameof(TextureWrappingMode::ClampToEdge)));
    wrapModeEnums.emplace_back("", 3, Prettify(nameof(TextureWrappingMode::MirrorClampToEdge)));
    wrapModeEnums.emplace_back("", 4, Prettify(nameof(TextureWrappingMode::ClampToBorder)));

    compareModeEnums.emplace_back("", 0, Prettify(nameof(TextureCompareMode::CompareRefToTexture)));
    compareModeEnums.emplace_back("", 1, Prettify(nameof(TextureCompareMode::None)));

    compareFunctionEnums.emplace_back("", 0, Prettify(nameof(TextureCompareFunction::LessEqual)));
    compareFunctionEnums.emplace_back("", 1, Prettify(nameof(TextureCompareFunction::GreaterEqual)));
    compareFunctionEnums.emplace_back("", 2, Prettify(nameof(TextureCompareFunction::Less)));
    compareFunctionEnums.emplace_back("", 3, Prettify(nameof(TextureCompareFunction::Greater)));
    compareFunctionEnums.emplace_back("", 4, Prettify(nameof(TextureCompareFunction::Equal)));
    compareFunctionEnums.emplace_back("", 5, Prettify(nameof(TextureCompareFunction::NotEqual)));
    compareFunctionEnums.emplace_back("", 6, Prettify(nameof(TextureCompareFunction::Always)));
    compareFunctionEnums.emplace_back("", 7, Prettify(nameof(TextureCompareFunction::Never)));
}

void SamplerDisplay::SetDisplayTarget(const AssetHandle samplerAssetHandle) noexcept {
    assert(samplerAssetHandle->IsSampler());
    if (samplerAsset != samplerAssetHandle) {
        samplerAsset = samplerAssetHandle;
    }
}
void SamplerDisplay::RenderMessagesTable() noexcept {
    assert(samplerAsset.HasValue());
    if (!samplerAsset->HasErrorMessages() &&
        !samplerAsset->HasWarningMessages() &&
        !samplerAsset->HasInfoMessages()) {
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    ImGui::BeginTable("compiler_logs", 2, flags);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableSetupColumn("Sampler Logs", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    ImGui::PushStyleColor(ImGuiCol_Text, ComponentDefinitionViewColors::ERROR_COLOR);
    for (auto& message : samplerAsset->ErrorMessages()) {
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
    for (auto& message : samplerAsset->WarningMessages()) {
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
    for (auto& message : samplerAsset->InfoMessages()) {
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
void SamplerDisplay::RenderSamplerParameters() noexcept {
    bool changed{ false };
    Sampler& sampler = samplerAsset->DataAs<Sampler>();

    { // Min Filter
        int minFilter{ static_cast<int>(sampler.MinFilter) };
        changed |= EnumWidget(Prettify(nameof(Sampler::MinFilter)), minFilter, minFilterEnums);
        sampler.MinFilter = static_cast<decltype(Sampler::MinFilter)>(minFilter);
    }
    { // Mag Filter
        int magFilter{ static_cast<int>(sampler.MagFilter) };
        changed |= EnumWidget(Prettify(nameof(Sampler::MagFilter)), magFilter, magFilterEnums);
        sampler.MagFilter = static_cast<decltype(Sampler::MagFilter)>(magFilter);
    }

    changed |=FloatWidget(Prettify(nameof(Sampler::MinLOD)), sampler.MinLOD);
    changed |=FloatWidget(Prettify(nameof(Sampler::MaxLOD)), sampler.MaxLOD);
    changed |=FloatWidget(Prettify(nameof(Sampler::LODBias)), sampler.LODBias);

    { // Wrap S
        int wrapS{ static_cast<int>(sampler.WrapS) };
        changed |= EnumWidget(Prettify(nameof(Sampler::WrapS)), wrapS, wrapModeEnums);
        sampler.WrapS = static_cast<decltype(Sampler::WrapS)>(wrapS);
    }
    { // Wrap T
        int wrapT{ static_cast<int>(sampler.WrapT) };
        changed |= EnumWidget(Prettify(nameof(Sampler::WrapT)), wrapT, wrapModeEnums);
        sampler.WrapT = static_cast<decltype(Sampler::WrapT)>(wrapT);
    }
    { // Wrap R
        int wrapR{ static_cast<int>(sampler.WrapR) };
        changed |= EnumWidget(Prettify(nameof(Sampler::WrapR)), wrapR, wrapModeEnums);
        sampler.WrapR = static_cast<decltype(Sampler::WrapR)>(wrapR);
    }

    changed |= ColorWidget(Prettify(nameof(Sampler::BorderColor)), sampler.BorderColor);

    { // Compare Mode
        int compareMode{ static_cast<int>(sampler.CompareMode) };
        changed |= EnumWidget(Prettify(nameof(Sampler::CompareMode)), compareMode, compareModeEnums);
        sampler.CompareMode = static_cast<decltype(Sampler::CompareMode)>(compareMode);
    }
    { // Compare Func
        int compareFunc{ static_cast<int>(sampler.CompareFunction) };
        changed |= EnumWidget(Prettify(nameof(Sampler::CompareFunction)), compareFunc, compareFunctionEnums);
        sampler.CompareFunction = static_cast<decltype(Sampler::CompareFunction)>(compareFunc);
    }

    changed |= FloatWidget(Prettify(nameof(Sampler::MaxAnisotropy)), sampler.MaxAnisotropy, 1.0f, 1.0f, 16.0f);
    changed |= BoolWidget(Prettify(nameof(Sampler::CubemapSeamless)), sampler.CubemapSeamless);

    if (changed) { samplerAsset->Serialize(); }
}
