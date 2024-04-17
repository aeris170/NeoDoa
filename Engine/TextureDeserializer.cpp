#include <Engine/TextureDeserializer.hpp>

#include <format>

#include <stb_image.h>

#include <Engine/Log.hpp>
#include <Engine/Assets.hpp>
#include <Engine/FileNode.hpp>

static RawData ToRawData(const std::string_view str) {
    RawData raw;
    raw.reserve(str.size());
    for (auto c : str) {
        raw.push_back(static_cast<std::byte>(c));
    }
    return raw;
}
TextureEncoding ExtToEncoding(const std::string_view ext) noexcept {
    using enum TextureEncoding;
    if (ext == Assets::TextureExtensionPNG) {
        return PNG;
    } else if (ext == Assets::TextureExtensionBMP) {
        return BMP;
    } else if (ext == Assets::TextureExtensionTGA) {
        return TGA;
    } else if(ext == Assets::TextureExtensionJPG || ext == Assets::TextureExtensionJPEG) {
        return JPG;
    } else {
        std::unreachable();
    }
}

TextureDeserializationResult DeserializeTexture(const FNode& file) {
    file.ReadContent();

    EncodedTextureData encodedData;
    encodedData.Name = file.Name();
    encodedData.EncodedData = ToRawData(file.DisposeContent());

    const auto& ext = file.Extension();
    encodedData.HasTransparency = ext == Assets::TextureExtensionPNG;
    encodedData.Encoding = ExtToEncoding(ext);

    return DeserializeTexture(encodedData);
}
TextureDeserializationResult DeserializeTexture(const EncodedTextureData& data) {
    TextureDeserializationResult rv;

    stbi_set_flip_vertically_on_load(true);
    const stbi_uc* stbi_encoded_data = reinterpret_cast<const stbi_uc*>(data.EncodedData.data());
    int data_length = static_cast<int>(data.EncodedData.size());
    int width, height, nrChannels;
    unsigned char* pixelData = stbi_load_from_memory(
        stbi_encoded_data,
        data_length,
        &width, &height,
        &nrChannels,
        STBI_default
    );
    if (pixelData == nullptr) {
        rv.erred = true;
        rv.errors.emplace_back(std::format("Couldn't load \"{}\" from memory!", data.Name));
        DOA_LOG_WARNING("Couldn't load \"%s\" from memory!", data.Name.c_str());
    } else {
        rv.deserializedTexture.Name = data.Name;
        rv.deserializedTexture.Width = width;
        rv.deserializedTexture.Height = height;
        rv.deserializedTexture.Channels = nrChannels;
        if (nrChannels == 1) {
            rv.deserializedTexture.Format = DataFormat::R8;
        } else if (nrChannels == 2) {
            rv.deserializedTexture.Format = DataFormat::RG8;
        } else if (nrChannels == 3) {
            rv.deserializedTexture.Format = DataFormat::RGB8;
        } else if (nrChannels == 4) {
            rv.deserializedTexture.Format = DataFormat::RGBA8;
        } else {
            std::unreachable();
        }
        rv.deserializedTexture.PixelData.resize(width * height * nrChannels);
        for (auto i = 0; i < rv.deserializedTexture.PixelData.size(); i++) {
            rv.deserializedTexture.PixelData[i] = static_cast<std::byte>(pixelData[i]);
        }
    }

    stbi_image_free(pixelData);
    return rv;
}
