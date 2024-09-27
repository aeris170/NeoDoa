#pragma once

#include <functional>

#include <Editor/DisplayTargetRenderer.hpp>

struct GUI;
struct Scene;
struct FNode;
struct Shader;
struct Component;
struct AssetHandle;

struct Observer {

    std::reference_wrapper<GUI> gui;

    explicit Observer(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

private:

    DisplayTargetRenderer displayTargetRenderer{ *this };

};
