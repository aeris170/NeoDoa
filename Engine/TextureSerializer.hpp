#pragma once

#include <optional>

#include <Engine/Texture.hpp>

std::optional<EncodedTextureData> SerializeTexture(const Texture& texture, TextureEncoding encoding = TextureEncoding::PNG);
