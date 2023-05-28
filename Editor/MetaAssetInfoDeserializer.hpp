#pragma once

#include <string>

#include <Engine/FileNode.hpp>

struct MetaAssetInfo;
struct MetaAssetInfoBank;

MetaAssetInfo DeserializeMetaAssetInfo(const FNode& file);
MetaAssetInfo DeserializeMetaAssetInfo(const std::string& data);

MetaAssetInfoBank DeserializeMetaAssetInfoBank(const FNode& file);
MetaAssetInfoBank DeserializeMetaAssetInfoBank(const std::string& data);