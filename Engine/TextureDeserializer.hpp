#pragma once

struct FNode;
struct Texture;
struct EncodedTextureData;

Texture DeserializeTexture(const FNode& file);
Texture DeserializeTexture(const EncodedTextureData& data);
