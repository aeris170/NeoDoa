#pragma once

#pragma region Libraries

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

#pragma region Assimp
#pragma endregion

#pragma region EnTT
#include <entt.hpp>
#pragma endregion

#pragma region EzEasing
#include <EZEasing.hpp>
#pragma endregion

#pragma region GLEW
#pragma endregion

#pragma region GLFW
#pragma endregion

#pragma region GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#pragma endregion

#pragma region IconFontCppHeaders
#pragma endregion

#pragma region [ImGui]zmo
#include <imgui.h>
#include <ImGuizmo.h>
#pragma endregion

#pragma region STB
#pragma endregion

#pragma region TinyFD
#pragma endregion

#pragma region TinyXML2
#pragma endregion

#pragma endregion

#pragma region Engine

#pragma region Core
#include <Core/Core.hpp>
#include <Core/Input.hpp>
#include <Core/Window.hpp>
#include <Core/FrameBuffer.hpp>
#pragma endregion

#pragma region Log
#include <Log/Log.hpp>
#pragma endregion

#pragma region ImGui
#include <ImGui/ImGuiRenderer.hpp>
#include <ImGui/ImGuiRenderCommand.hpp>
#pragma endregion

#pragma region Renderer
#include <Renderer/Renderer.hpp>
#pragma endregion

#pragma region Project
#pragma region Scene
#pragma region Serialize
#include <Project/Scene/Serialize/SceneSerializer.hpp>
#include <Project/Scene/Serialize/SceneDeserializer.hpp>
#pragma endregion
#include <Project/Scene/Scene.hpp>
#include <Project/Scene/OrthoCamera.hpp>
#include <Project/Scene/PerspectiveCamera.hpp>
#pragma endregion
#include <Project/Project.hpp>
#pragma endregion

#pragma region Asset
#pragma region Manager
#include <Asset/Manager/FileNode.hpp>
#include <Asset/Assets.hpp>
#pragma endregion
#pragma region Color
#include <Asset/Color/Color.hpp>
#pragma endregion
#pragma region Model
#include <Vertex.hpp>
#include <Mesh.hpp>
#include <Model.hpp>
#pragma endregion
#pragma region ScriptComponent
#pragma region Modules
#include <ModelRenderer.hpp>
#pragma endregion
#include <Angel.hpp>
#include <PropertyData.hpp>
#include <EnumValue.hpp>
#pragma endregion
#pragma region Shader
#include <Shader.hpp>
#pragma endregion
#pragma region Texture
#include <Texture.hpp>
#pragma endregion
#pragma endregion

#pragma endregion