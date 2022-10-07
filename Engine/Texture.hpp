#pragma once

#include <memory>

#include "TypedefsAndConstants.hpp"

struct Texture : std::enable_shared_from_this<Texture> {
    std::string _name;
    TEX _glTextureID{ 0 };
    std::vector<unsigned char> _pixelData;

    void Bind(int slot = 0);

    // don't call use CreateTexture
    Texture(std::string_view name, int width, int height, const unsigned char* const pixelData, bool hasTransparency) noexcept;
    ~Texture() noexcept;
    Texture(const Texture&) = delete;
    Texture(Texture&&) noexcept;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) noexcept;

private:
    static bool FACTORY_FLAG;

    friend std::weak_ptr<Texture> CreateTexture(std::string_view name, const char* path, bool hasTransparency);
    friend std::weak_ptr<Texture> CreateTexture(std::string_view name, const unsigned char* data, int length, bool hasTransparency);
    friend std::weak_ptr<Texture> CreateTextureRaw(std::string_view name, const unsigned char* data, int width, int height, bool hasTransparency);
};

std::weak_ptr<Texture> CreateTexture(std::string_view name, const char* path, bool hasTransparency = true);
std::weak_ptr<Texture> CreateTexture(std::string_view name, const unsigned char* data, int length, bool hasTransparency = true);
std::weak_ptr<Texture> CreateTextureRaw(std::string_view name, const unsigned char* data, int width, int height, bool hasTransparency = true);
std::weak_ptr<Texture> FindTexture(std::string_view name);
