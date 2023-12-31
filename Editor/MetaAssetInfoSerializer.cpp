#include "MetaAssetInfoSerializer.hpp"

#include <tinyxml2.h>

#include <Utility/NameOf.hpp>

#include "MetaAssetInfo.hpp"

std::string SerializeMetaAssetInfo(const MetaAssetInfo& info) {
    tinyxml2::XMLPrinter printer;

    printer.OpenElement("info");
    {
        printer.PushAttribute(nameof_c(MetaAssetInfo::file), info.file->Path().string().c_str());
        printer.PushAttribute(nameof_c(MetaAssetInfo::icon_index), info.icon_index);
        printer.PushAttribute(nameof_c(MetaAssetInfo::fa_icon), info.fa_icon);
        printer.PushAttribute(nameof_c(MetaAssetInfo::svg_icon_key), info.svg_icon_key.c_str());
    }
    printer.CloseElement(); // info close
    return printer.CStr();
}
std::string SerializeMetaAssetInfoBank(const MetaAssetInfoBank& bank) {
    tinyxml2::XMLPrinter printer;

    printer.PushComment("WARNING!! This file is not for editing! Don't!");
    printer.OpenElement("bank");
    {
        tinyxml2::XMLDocument doc;
        for (const auto& [file, info] : bank) {
            doc.Parse(SerializeMetaAssetInfo(info).c_str());
            doc.Accept(&printer);
            doc.Clear();
        }
    }
    printer.CloseElement(); // bank close
    return printer.CStr();
}