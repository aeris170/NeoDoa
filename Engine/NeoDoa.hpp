#pragma once

#pragma region Libs
#pragma region GLEW
#include <GL/glew.h>
#pragma endregion
#pragma region GLFW
#include <GLFW/glfw3.h>
#pragma endregion
#pragma region [ImGui]zmo
#include <imgui.h>
#include <ImGuizmo.h>
#pragma endregion
#pragma region GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma endregion
#pragma region EnTT
#include <entt.hpp>
#pragma endregion
#pragma region AngelScript
#include <angelscript.h>
#include <contextmgr/contextmgr.h>
#include <datetime/datetime.h>
#include <debugger/debugger.h>
#include <scriptany/scriptany.h>
#include <scriptarray/scriptarray.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptfile/scriptfile.h>
#include <scriptgrid/scriptgrid.h>
#include <scripthandle/scripthandle.h>
#include <scripthelper/scripthelper.h>
#include <scriptmath/scriptmath.h>
#include <scriptstdstring/scriptstdstring.h>
#include <serializer/serializer.h>
#include <weakref/weakref.h>
#pragma endregion
#pragma endregion

#pragma region Engine
#pragma region Core
#include "Core.hpp"
#include "Input.hpp"
#include "Window.hpp"
#include "FrameBuffer.hpp"
#include "Log.hpp"
#pragma endregion
#pragma region ECS
#include "Tag.hpp"
#include "Transform.hpp"
#include "ModelRenderer.hpp"
#include "ScriptComponent.hpp"
#pragma endregion
#pragma region ImGui
#include "ImGuiRenderer.hpp"
#include "ImGuiRenderCommand.hpp"
#pragma endregion
#pragma region Renderer
#include "Mesh.hpp"
#include "Model.hpp"
#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"
#pragma endregion
#pragma region Scene
#include "Scene.hpp"
#include "Camera.hpp"
#include "OrthoCamera.hpp"
#include "PerspectiveCamera.hpp"
#pragma endregion
#pragma region Angel
#include "Angel.hpp"
#include "PropertyData.hpp"
#pragma endregion
#pragma endregion