#pragma once

#include "Texture.hpp"

struct SerializedTexture {

    std::string name;

    void* data;
    int dataSize;

    bool hasTransparency;
};

SerializedTexture SerializeTexture(const Texture& texture);