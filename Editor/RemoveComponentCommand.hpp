#pragma once

#include <format>
#include <optional>
#include <string_view>
#include <type_traits>

#include <Engine/Entity.hpp>

#include <Editor/GUI.hpp>
#include <Editor/GUICommand.hpp>

template<typename T> requires std::is_nothrow_move_assignable_v<T>
struct RemoveComponentCommand : GUICommand {
    explicit RemoveComponentCommand(GUI& gui, Entity componentOwner, std::string_view componentNameInDescription = "a component") noexcept :
        GUICommand(gui),
        entity(componentOwner) {
        description = std::format("Remove {} from Entity with ID: {}", componentNameInDescription.data(), EntityTo<uint32_t>(componentOwner));
    }
    void Execute() noexcept override {
        assert(gui.HasOpenScene());
        assert(gui.GetOpenScene().template HasComponent<T>(entity));
        assert(!removedComponent.has_value());
        removedComponent = std::move(gui.GetOpenScene().template GetComponent<T>(entity));
        gui.GetOpenScene().template RemoveComponent<T>(entity);
    }
    void UnExecute() noexcept override {
        assert(gui.HasOpenScene());
        assert(!gui.GetOpenScene().template HasComponent<T>(entity));
        assert(removedComponent.has_value());
        gui.GetOpenScene().template InsertComponent<T>(entity, std::move(removedComponent.value()));
    }
private:
    Entity entity;
    std::optional<T> removedComponent{};
};