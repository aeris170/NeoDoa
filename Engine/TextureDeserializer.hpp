#pragma once

struct FNode;
struct Texture;
struct TextureData;

Texture DeserializeTexture(const FNode& file);
Texture DeserializeTexture(const TextureData& data);
