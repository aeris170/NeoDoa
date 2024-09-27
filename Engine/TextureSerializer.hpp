#pragma once

#include <Engine/Texture.hpp>

EncodedTextureData SerializeTexture(const Texture& texture, TextureEncoding encoding = TextureEncoding::PNG);
