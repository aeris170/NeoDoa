#include <Engine/TextureDeserializer.hpp>

#include <Engine/Texture.hpp>
#include <Engine/FileNode.hpp>
#include <Engine/TextureSerializer.hpp>

Texture DeserializeTexture(const FNode& file) { return Texture::CreateTexture(file.Name(), file.AbsolutePath().string().c_str()); }
Texture DeserializeTexture(const EncodedTextureData& data) {
    return Texture::CreateTexture(
        data.Name,
        data.Data,
        data.HasTransparency ? TextureTransparency::YES : TextureTransparency::NO
    );
}
