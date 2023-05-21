#pragma once

#include "Texture.hpp"

EncodedTextureData SerializeTexture(const Texture& texture, TextureEncoding encoding = TextureEncoding::PNG);
