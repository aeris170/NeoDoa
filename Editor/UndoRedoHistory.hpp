#pragma once

#include <functional>

struct GUI;

struct UndoRedoHistory {
    std::reference_wrapper<GUI> gui;

    explicit UndoRedoHistory(GUI& owner) noexcept;

    bool Begin() noexcept;
    void Render() noexcept;
    void End() noexcept;

    void Show() noexcept;
    void Hide() noexcept;

private:
    bool isOpen{ false };
    bool isClosing{ false };
    bool isMouseCaught{ false };
    int stackDelta{};
};