#pragma once

#include "TypedefsAndConstants.hpp"

struct GLFWwindow;
struct ImGuiContext;

ImGuiContext* ImGuiInit(GLFWwindow* window);

void ImGuiRender(float delta);
void ImGuiAddRenderCommand(ImGuiFunction function);

void ImGuiClean();
