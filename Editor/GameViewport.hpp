#pragma once

#include <vector>
#include <functional>
#include <string_view>

#include <Engine/UUID.hpp>
#include <Engine/Assets.hpp>
#include <Engine/Entity.hpp>

struct GUI;

struct GameViewport {

    std::reference_wrapper<GUI> gui;

    explicit GameViewport(GUI& gui) noexcept;


    bool Begin();
    void Render();
    void End();

private:

    static constexpr auto ColorAttachment0{ "Color Attachment 0" };
    static constexpr auto ColorAttachment1{ "Color Attachment 1" };
    static constexpr auto ColorAttachment2{ "Color Attachment 2" };
    static constexpr auto ColorAttachment3{ "Color Attachment 3" };
    static constexpr auto ColorAttachment4{ "Color Attachment 4" };
    static constexpr auto ColorAttachment5{ "Color Attachment 5" };
    static constexpr auto ColorAttachment6{ "Color Attachment 6" };
    static constexpr auto ColorAttachment7{ "Color Attachment 7" };
    static constexpr auto DepthAttachment{ "Depth Attachment" };
    static constexpr auto DepthStencilAttachment{ "Depth-Stencil Attachment (Stencil values not displayed)" };

    UUID selectedFrameBufferID{ UUID::Empty() };
    std::vector<std::string_view> frameBufferAssetNames{};

    unsigned selectedAttachmentIndex{}; // index into displayableAttachmentNames
    std::vector<std::string_view> displayableAttachmentNames{};

    static const GPUTexture& FetchAttachmentTexture(const Assets& assets, UUID frameBufferAssetID, std::string_view attachmentName) noexcept;

    void OnProjectLoaded(Project& project) noexcept;
    void OnProjectUnloaded() noexcept;
    void OnReimport(Assets& assets) noexcept;
    void OnAssetCreated(AssetHandle handle) noexcept;
    void OnAssetDeleted(AssetHandle handle) noexcept;
};
