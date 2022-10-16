#pragma once

struct FNode;
struct Texture;
struct SerializedTexture;

Texture DeserializeTexture(const FNode& file);
Texture DeserializeTexture(const SerializedTexture& data);
