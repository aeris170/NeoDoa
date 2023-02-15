#include "MetaAssetInfo.hpp"

void* MetaAssetInfo::GetSVGIcon(TextureStyle style) const { return SVGPathway::Get(svg_icon_key, style).TextureIDRaw(); }