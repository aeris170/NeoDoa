#pragma once

#include <functional>

#include <Engine/Assets.hpp>

#include <Editor/TextEditor.hpp>

struct Observer;

struct ComponentDefinitionDisplay {

    ComponentDefinitionDisplay(Observer& observer) noexcept;

    void SetDisplayTarget(const AssetHandle componentDefAssetHandle) noexcept;

    void RenderMessagesTable() noexcept;
    void RenderFields() noexcept;
    void RenderSourceCode() noexcept;

private:
    std::reference_wrapper<Observer> observer;

    AssetHandle componentDefAsset{};
    TextEditor textEditorInstance{};
};