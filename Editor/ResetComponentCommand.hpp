#pragma once

#include <format>
#include <optional>
#include <string_view>
#include <type_traits>

#include <Engine/Entity.hpp>

#include <Editor/GUI.hpp>
#include <Editor/GUICommand.hpp>

#include <Engine/MultiMaterialComponent.hpp>

template<typename T> requires std::is_nothrow_copy_assignable_v<T> && std::is_nothrow_move_assignable_v<T>
struct ResetComponentCommand : GUICommand {
    explicit ResetComponentCommand(GUI& gui, Entity componentOwner, std::string_view componentNameInDescription = "a component") noexcept :
        GUICommand(gui),
        entity(componentOwner) {
        description = std::format("Reset {} of Entity with ID: {}", componentNameInDescription.data(), EntityTo<uint32_t>(componentOwner));
    }
    void Execute() noexcept override {
        assert(gui.HasOpenScene());
        assert(gui.GetOpenScene().template HasComponent<T>(entity));
        assert(!resettedComponent.has_value());
        T& t = gui.GetOpenScene().template GetComponent<T>(entity);
        resettedComponent = std::move(t);
        t = T(entity);
    }
    void UnExecute() noexcept override {
        assert(gui.HasOpenScene());
        assert(gui.GetOpenScene().template HasComponent<T>(entity));
        assert(resettedComponent.has_value());
        T& t = gui.GetOpenScene().template GetComponent<T>(entity);
        t = std::move(resettedComponent.value());
    }
private:
    Entity entity;
    std::optional<T> resettedComponent{};
};