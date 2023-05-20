#pragma once

#include <functional>

struct GUI;

struct GameViewport {

    std::reference_wrapper<GUI> gui;

    explicit GameViewport(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

};
