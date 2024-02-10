#pragma once

#include <string>
#include <string_view>

#include <imgui.h>

struct GUI;

struct ErrorModal {

    explicit ErrorModal(GUI& gui) noexcept;

    void Show(std::string_view errorText) const noexcept;
    void Hide() const noexcept;
    bool IsVisible() const noexcept;

private:
    std::reference_wrapper<GUI> gui;
    mutable bool isModalActive{ false };
    mutable bool isModalOpen{ true };
    mutable std::string errorText{};

    void Render() noexcept;

    friend struct GUI;

    static constexpr auto TITLE_TEXT{ "ERROR" };
    static constexpr auto OK_BUTTON_TEXT{ "OK" };
};