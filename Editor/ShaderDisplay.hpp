#pragma once

#include <functional>

#include <Engine/Assets.hpp>

#include <Editor/TextEditor.hpp>

struct Observer;

struct ShaderDisplay {

    ShaderDisplay(Observer& observer) noexcept;

    void SetDisplayTarget(const AssetHandle shaderAssetHandle) noexcept;

    void RenderMessagesTable() noexcept;
    void RenderFields() noexcept;
    void RenderSourceCode() noexcept;

private:
    std::reference_wrapper<Observer> observer;

    uint64_t assetVersion{};
    AssetHandle shaderAsset{};
    TextEditor textEditorInstance{};
};