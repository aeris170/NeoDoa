#pragma once

#include <string>

#include "Component.hpp"

struct FNode;

enum class ComponentCompilerMessageType {
    INFO,
    WARNING,
    ERROR,
    ComponentCompilerMessageType_COUNT
};
struct ComponentCompilerMessage {
    int lineNo, columnNo;
    ComponentCompilerMessageType messageType;
    std::string message;
};
struct ComponentDeserializationResult {
    bool erred{ false };
    std::vector<ComponentCompilerMessage> messages{};
    Component deserializedComponent;
};

ComponentDeserializationResult DeserializeComponent(const FNode& file);
ComponentDeserializationResult DeserializeComponent(const std::string_view data);