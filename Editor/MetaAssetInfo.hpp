#pragma once

#include <string>

#include <UUID.hpp>
#include <FileNode.hpp>

#include "SVGPathway.hpp"

struct MetaAssetInfo {
    const FNode* assetID;

    int icon_index;
    const char* fa_icon; /* from IconsFontAwesome6Pro.h */
    std::string svg_icon_key; /* for SVGPathway::Get */

    void* GetSVGIcon(TextureStyle style = TextureStyle::PADDED) const;
};