#include <Editor/MetaAssetInfo.hpp>

#include <tinyxml2.h>

#include <Engine/Core.hpp>
#include <Engine/FileNode.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/MetaAssetInfoSerializer.hpp>
#include <Editor/MetaAssetInfoDeserializer.hpp>

void* MetaAssetInfo::GetSVGIcon(TextureStyle style) const { return SVGPathway::Get(svg_icon_key, style).TextureIDRaw(); }

void MetaAssetInfoBank::SaveToDisk(const MetaAssetInfoBank& bank, const FNode& editorMetaFolder) noexcept {
    tinyxml2::XMLDocument doc;
    doc.Parse(SerializeMetaAssetInfoBank(bank).c_str());
    doc.SaveFile((editorMetaFolder.AbsolutePath() / BankFileName).string().c_str());
}
void MetaAssetInfoBank::LoadFromDisk(MetaAssetInfoBank& bank, FNode& editorMetaFolder) noexcept {
    auto bankFile = editorMetaFolder.CreateChildFileIfNotExists({
        .name = BankFileName,
    });
    if (!bankFile) {
        bankFile = &editorMetaFolder.FindChild(BankFileName);
    }

    bank.Clear();
    auto result = DeserializeMetaAssetInfoBank(*bankFile);
    if (!result.erred) {
        bank = std::move(result.bank);
    } else {
        static auto& Core = Core::GetCore();
        const auto& assets = Core->Assets();
        for (const auto& assetID : assets->AllAssetsIDs()) {
            AssetHandle handle = assets->FindAsset(assetID);
            assert(handle.HasValue());
            bank.TryEmplace(handle->File(), {});
        }
        SaveToDisk(bank, editorMetaFolder);
    }
}

void MetaAssetInfoBank::Clear() {
    metaInfo.clear();
}
void MetaAssetInfoBank::Emplace(MetaAssetInfo&& emplacee) { metaInfo.try_emplace(emplacee.file, std::move(emplacee)); }
MetaAssetInfo& MetaAssetInfoBank::GetMetaInfoOf(const FNode& file, const MetaAssetInfo& emplaceThisIfAbsent) {
    TryEmplace(file, emplaceThisIfAbsent);
    return metaInfo[&file];
}

MetaAssetInfoBank::DataStructure::iterator MetaAssetInfoBank::begin() noexcept { return metaInfo.begin(); }
MetaAssetInfoBank::DataStructure::iterator MetaAssetInfoBank::end() noexcept { return metaInfo.end(); }
MetaAssetInfoBank::DataStructure::const_iterator MetaAssetInfoBank::begin() const noexcept { return metaInfo.begin(); }
MetaAssetInfoBank::DataStructure::const_iterator MetaAssetInfoBank::end() const noexcept { return metaInfo.end(); }

void MetaAssetInfoBank::TryEmplace(const FNode& file, const MetaAssetInfo& emplaceThisIfAbsent) {
    const char* fa_icon{ nullptr };
    std::string svg_icon_key{};

    if (file.IsDirectory()) {
        const auto& [_fa_icon, _svg_icon_key] = FileIcons::DirectoryIcons[0];
        fa_icon = _fa_icon;
        svg_icon_key = _svg_icon_key;
    } else {
        AssetHandle handle = Core::GetCore()->Assets()->FindAssetAt(file);
        assert(handle.HasValue());
        if (handle->IsScene()) {
            const auto& [_fa_icon, _svg_icon_key] = FileIcons::SceneIcons[0];
            fa_icon = _fa_icon;
            svg_icon_key = _svg_icon_key;
        } else if (handle->IsComponentDefinition()) {
            const auto& [_fa_icon, _svg_icon_key] = FileIcons::ComponentIcons[0];
            fa_icon = _fa_icon;
            svg_icon_key = _svg_icon_key;
        } else if (handle->IsShader()) {
            if (Assets::IsVertexShaderFile(handle->File())) {
                const auto& [_fa_icon, _svg_icon_key] = FileIcons::ShaderIcons[1];
                fa_icon = _fa_icon;
                svg_icon_key = _svg_icon_key;
            }
            if (Assets::IsTessellationControlShaderFile(handle->File())) {
                const auto& [_fa_icon, _svg_icon_key] = FileIcons::ShaderIcons[2];
                fa_icon = _fa_icon;
                svg_icon_key = _svg_icon_key;
            }
            if (Assets::IsTessellationEvaluationShaderFile(handle->File())) {
                const auto& [_fa_icon, _svg_icon_key] = FileIcons::ShaderIcons[3];
                fa_icon = _fa_icon;
                svg_icon_key = _svg_icon_key;
            }
            if (Assets::IsGeometryShaderFile(handle->File())) {
                const auto& [_fa_icon, _svg_icon_key] = FileIcons::ShaderIcons[4];
                fa_icon = _fa_icon;
                svg_icon_key = _svg_icon_key;
            }
            if (Assets::IsFragmentShaderFile(handle->File())) {
                const auto& [_fa_icon, _svg_icon_key] = FileIcons::ShaderIcons[5];
                fa_icon = _fa_icon;
                svg_icon_key = _svg_icon_key;
            }
            if (Assets::IsComputeShaderFile(handle->File())) {
                const auto& [_fa_icon, _svg_icon_key] = FileIcons::ShaderIcons[6];
                fa_icon = _fa_icon;
                svg_icon_key = _svg_icon_key;
            }
        } else if (handle->IsShaderProgram()) {
            const auto& [_fa_icon, _svg_icon_key] = FileIcons::ShaderProgramIcons[0];
            fa_icon = _fa_icon;
            svg_icon_key = _svg_icon_key;
        } else if (handle->IsTexture()) {
            const auto& [_fa_icon, _svg_icon_key] = FileIcons::TextureIcons[0];
            fa_icon = _fa_icon;
            svg_icon_key = _svg_icon_key;
        } else {
            fa_icon = ICON_FA_FILE;
            svg_icon_key = GUI::FILE_ICON_KEY;
        }
    }

    MetaAssetInfo emplaceThisIfAbsentCopy = emplaceThisIfAbsent;
    emplaceThisIfAbsentCopy.file = &file;
    emplaceThisIfAbsentCopy.icon_index = 0;
    emplaceThisIfAbsentCopy.fa_icon = fa_icon;
    emplaceThisIfAbsentCopy.svg_icon_key = std::move(svg_icon_key);
    metaInfo.try_emplace(&file, emplaceThisIfAbsentCopy);
}