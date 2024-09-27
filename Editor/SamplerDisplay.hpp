#pragma once

#include <vector>
#include <functional>

#include <Engine/Assets.hpp>
#include <Engine/EnumValue.hpp>

struct Observer;

struct SamplerDisplay {

    explicit SamplerDisplay(Observer& observer) noexcept;

    void SetDisplayTarget(const AssetHandle samplerAssetHandle) noexcept;
    void RenderMessagesTable() noexcept;
    void RenderSamplerParameters() noexcept;

private:
    std::reference_wrapper<Observer> observer;

    AssetHandle samplerAsset{};
    std::vector<EnumValue> minFilterEnums{};
    std::vector<EnumValue> magFilterEnums{};
    std::vector<EnumValue> wrapModeEnums{};
    std::vector<EnumValue> compareModeEnums{};
    std::vector<EnumValue> compareFunctionEnums{};
};