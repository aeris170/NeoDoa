#pragma once

#include <variant>

#include <Engine/Graphics.hpp>
#include <Engine/Resolution.hpp>

struct FrameBufferAttachment {
    Resolution Resolution{ 1920, 1080 };
    DataFormat Format{};
    Multisample Samples{ Multisample::None };
    bool HasReadbackSupport{ false };
};

struct FrameBuffer {

    std::string Name{};
    std::vector<FrameBufferAttachment> ColorAttachments{};
    std::optional<FrameBufferAttachment> DepthAttachment{};
    std::optional<FrameBufferAttachment> StencilAttachment{};
    std::optional<FrameBufferAttachment> DepthStencilAttachment{};

    std::string Serialize() const noexcept;
    static FrameBuffer Deserialize(const std::string_view data) noexcept;

    static FrameBuffer Copy(const FrameBuffer& sampler) noexcept;
};
