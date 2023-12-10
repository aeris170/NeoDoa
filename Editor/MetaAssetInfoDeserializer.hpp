#pragma once

#include <string>

#include <Engine/FileNode.hpp>

#include <Editor/MetaAssetInfo.hpp>

struct MetaAssetInfo;
struct MetaAssetInfoBank;
struct MetaAssetInfoBankDeserializationResult {
    bool erred{ false };
    MetaAssetInfoBank bank;
};

MetaAssetInfo DeserializeMetaAssetInfo(const FNode& file);
MetaAssetInfo DeserializeMetaAssetInfo(const std::string& data);

MetaAssetInfoBankDeserializationResult DeserializeMetaAssetInfoBank(const FNode& file);
MetaAssetInfoBankDeserializationResult DeserializeMetaAssetInfoBank(const std::string& data);