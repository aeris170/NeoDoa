#include <Editor/MetaAssetInfoDeserializer.hpp>

#include <cstring>

#include <tinyxml2.h>

#include <Utility/NameOf.hpp>

#include <Engine/Log.hpp>
#include <Engine/Core.hpp>
#include <Engine/Assets.hpp>

#include <Editor/Icons.hpp>

static const char* FindIconInIcons(const char* icon);

MetaAssetInfo DeserializeMetaAssetInfo(const FNode& file) {
    file.ReadContent();
    return DeserializeMetaAssetInfo(file.DisposeContent());
}
MetaAssetInfo DeserializeMetaAssetInfo(const std::string& data) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.c_str());
    if (err != tinyxml2::XML_SUCCESS) {
        DOA_LOG_WARNING("Couldn't deserialize meta asset info!\n\n%s", data);
    }

    const auto* rootElement = doc.RootElement();
    return {
        &Core::GetCore()->GetAssets()->Root().FindChild(rootElement->FindAttribute(nameof_c(MetaAssetInfo::file))->Value()),
        rootElement->FindAttribute(nameof_c(MetaAssetInfo::icon_index))->IntValue(),
        FindIconInIcons(rootElement->FindAttribute(nameof_c(MetaAssetInfo::fa_icon))->Value()),
        rootElement->FindAttribute(nameof_c(MetaAssetInfo::svg_icon_key))->Value()
    };
}

MetaAssetInfoBankDeserializationResult DeserializeMetaAssetInfoBank(const FNode& file) {
    file.ReadContent();
    return DeserializeMetaAssetInfoBank(file.DisposeContent());
}
MetaAssetInfoBankDeserializationResult DeserializeMetaAssetInfoBank(const std::string& data) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.c_str());
    if (err != tinyxml2::XML_SUCCESS) {
        DOA_LOG_WARNING("Couldn't deserialize meta asset info bank! All custom set icons are, sadly, gone. You must re-apply your custom asset icons. %s", data);
        return { true, {} };
    }

    MetaAssetInfoBankDeserializationResult rv;
    MetaAssetInfoBank& bank = rv.bank;

    auto* bankElement = doc.RootElement();
    for (auto* infoElem = bankElement->FirstChildElement(); infoElem != nullptr; infoElem = infoElem->NextSiblingElement()) {
        tinyxml2::XMLPrinter printer;
        infoElem->Accept(&printer);
        MetaAssetInfo info{ DeserializeMetaAssetInfo(printer.CStr()) };
        bank.Emplace(std::move(info));
    }

    return rv;
};

static const char* FindIconInIcons(const char* icon) {
    for (const auto& [fa_icon, _] : FileIcons::DirectoryIcons) {
        if (std::strcmp(fa_icon, icon) == 0) {
            return fa_icon;
        }
    }
    for (const auto& [fa_icon, _] : FileIcons::SceneIcons) {
        if (std::strcmp(fa_icon, icon) == 0) {
            return fa_icon;
        }
    }
    for (const auto& [fa_icon, _] : FileIcons::TextureIcons) {
        if (std::strcmp(fa_icon, icon) == 0) {
            return fa_icon;
        }
    }
    for (const auto& [fa_icon, _] : FileIcons::ComponentIcons) {
        if (std::strcmp(fa_icon, icon) == 0) {
            return fa_icon;
        }
    }
    for (const auto& [fa_icon, _] : FileIcons::RegularFileIcons) {
        if (std::strcmp(fa_icon, icon) == 0) {
            return fa_icon;
        }
    }
    assert(false); // Icon not found, icon must be found!
    return nullptr;
}