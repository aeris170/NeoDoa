#pragma once

#include <string>

struct MetaAssetInfo;
struct MetaAssetInfoBank;

std::string SerializeMetaAssetInfo(const MetaAssetInfo& info);
std::string SerializeMetaAssetInfoBank(const MetaAssetInfoBank& bank);