#pragma once

#include <string>
#include <string_view>

#include <imgui.h>

struct GUI;

struct ErrorModal {

    explicit ErrorModal(GUI& gui) noexcept;

    void Show(std::string_view errorText) const;
    void Hide() const;

private:
    std::reference_wrapper<GUI> gui;
    mutable bool isModalActive{ false };
    mutable bool isModalOpen{ true };
    mutable std::string errorText{};

    void Render();

    friend struct GUI;

    static constexpr auto TITLE_TEXT{ "ERROR" };
    static constexpr auto OK_BUTTON_TEXT{ "OK" };
};