#include "TextureDeserializer.hpp"

#include "FileNode.hpp"
#include "Texture.hpp"
#include "TextureSerializer.hpp"

Texture DeserializeTexture(const FNode& file) { return Texture::CreateTexture(file.Name(), file.AbsolutePath().string().c_str()); }
Texture DeserializeTexture(const SerializedTexture& data) {
    return Texture::CreateTexture(data.name,
                                  reinterpret_cast<const unsigned char*>(data.data),
                                  data.dataSize,
                                  data.hasTransparency ? Texture::Transparency::YES : Texture::Transparency::NO);
}
