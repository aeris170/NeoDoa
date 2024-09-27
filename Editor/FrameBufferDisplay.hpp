#pragma once

#include <functional>

#include <Engine/Assets.hpp>
#include <Engine/EnumValue.hpp>

struct Observer;

struct FrameBufferDisplay {

    explicit FrameBufferDisplay(Observer& observer) noexcept;

    void SetDisplayTarget(const AssetHandle samplerAssetHandle) noexcept;
    void RenderMessagesTable() noexcept;
    void RenderColorAttachments() noexcept;
    void RenderDepthAttachment() noexcept;
    void RenderStencilAttachment() noexcept;
    void RenderDepthStencilAttachment() noexcept;

private:
    std::reference_wrapper<Observer> observer;

    AssetHandle frameBufferAsset{};

    std::vector<EnumValue> colorFormatEnums{};
    std::vector<EnumValue> depthFormatEnums{};
    std::vector<EnumValue> stencilFormatEnums{};
    std::vector<EnumValue> depthStencilFormatEnums{};
    std::vector<EnumValue> multisampleEnums{};

    bool RenderAttachment(FrameBufferAttachment& attachment, const std::vector<EnumValue>& formats) noexcept;

    enum AddRemoveResult {
        None,
        Add,
        Remove
    };
    AddRemoveResult RenderAddRemoveButtons(unsigned minElements, unsigned currentElements, unsigned maxElements) noexcept;
};
