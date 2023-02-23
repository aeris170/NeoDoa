#include "MetaAssetInfo.hpp"

#include <tinyxml2.h>

#include <Core.hpp>
#include <FileNode.hpp>

#include "GUI.hpp"
#include "Icons.hpp"
#include "MetaAssetInfoSerializer.hpp"
#include "MetaAssetInfoDeserializer.hpp"

void* MetaAssetInfo::GetSVGIcon(TextureStyle style) const { return SVGPathway::Get(svg_icon_key, style).TextureIDRaw(); }

void MetaAssetInfoBank::Clear() {
    metaInfo.clear();
}
void MetaAssetInfoBank::Emplace(MetaAssetInfo&& emplacee) { metaInfo.try_emplace(emplacee.file, std::move(emplacee)); }
MetaAssetInfo& MetaAssetInfoBank::GetMetaInfoOf(const FNode& file, const MetaAssetInfo& emplaceThisIfAbsent) {
    if (bankInfo.file == &file) { return bankInfo; }
    TryEmplace(file, emplaceThisIfAbsent);
    return metaInfo[&file];
}

MetaAssetInfoBank::DataStructure::iterator MetaAssetInfoBank::begin() { return metaInfo.begin(); }
MetaAssetInfoBank::DataStructure::iterator MetaAssetInfoBank::end() { return metaInfo.end(); }
MetaAssetInfoBank::DataStructure::const_iterator MetaAssetInfoBank::begin() const { return metaInfo.begin(); }
MetaAssetInfoBank::DataStructure::const_iterator MetaAssetInfoBank::end() const { return metaInfo.end(); }

void MetaAssetInfoBank::SaveToDisk(const std::filesystem::path& path) const {
    tinyxml2::XMLDocument doc;
    doc.Parse(SerializeMetaAssetInfoBank(*this).c_str());
    doc.SaveFile((path / "metaAssetInfo.bank").string().c_str());
}

void MetaAssetInfoBank::LoadFromDisk(const Project& owner) {
    FNode f{ {
        .owner = &owner,
        .name = (owner.Workspace() / "metaAssetInfo").string(),
        .ext = ".bank"
    } };
    *this = DeserializeMetaAssetInfoBank(f);
}

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