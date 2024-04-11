#pragma once

#include <Engine/Texture.hpp>
#include <Engine/GPUTexture.hpp>

TextureEncoding ExtToEncoding(const std::string_view ext) noexcept;

struct FNode;

struct TextureDeserializationResult {
    bool erred{ false };
    std::vector<TextureAllocatorMessage> errors{};
    Texture deserializedTexture;
};

TextureDeserializationResult DeserializeTexture(const FNode& file);
TextureDeserializationResult DeserializeTexture(const EncodedTextureData& data);
