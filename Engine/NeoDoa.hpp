#pragma once

#pragma region Libraries

#pragma region AngelScript
#include <angelscript.h>
#include <angelscript/contextmgr/contextmgr.h>
#include <angelscript/datetime/datetime.h>
#include <angelscript/debugger/debugger.h>
#include <angelscript/scriptany/scriptany.h>
#include <angelscript/scriptarray/scriptarray.h>
#include <angelscript/scriptbuilder/scriptbuilder.h>
#include <angelscript/scriptdictionary/scriptdictionary.h>
#include <angelscript/scriptfile/scriptfile.h>
#include <angelscript/scriptgrid/scriptgrid.h>
#include <angelscript/scripthandle/scripthandle.h>
#include <angelscript/scripthelper/scripthelper.h>
#include <angelscript/scriptmath/scriptmath.h>
#include <angelscript/scriptstdstring/scriptstdstring.h>
#include <angelscript/serializer/serializer.h>
#include <angelscript/weakref/weakref.h>
#pragma endregion

#pragma region Assimp
#pragma endregion

#pragma region EnTT
#include <entt/entt.hpp>
#pragma endregion

#pragma region EzEasing
#include <EZEasing.hpp>
#pragma endregion

#pragma region GLEW
#pragma endregion

#pragma region GLFW
#pragma endregion

#pragma region GLM
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
// define extra conversion here before including imgui, don't do it in the imconfig.h
#define IM_VEC2_CLASS_EXTRA \
    constexpr ImVec2(glm::vec2& f) : x(f.x), y(f.y) {} \
    operator glm::vec2() const { return glm::vec2(x, y); }

#define IM_VEC4_CLASS_EXTRA \
        constexpr ImVec4(const glm::vec4& f) : x(f.x), y(f.y), z(f.z), w(f.w) {} \
        operator glm::vec4() const { return glm::vec4(x,y,z,w); }

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
#include <Engine/Core.hpp>
#include <Engine/Input.hpp>
#include <Engine/Window.hpp>
#pragma endregion

#pragma region Log
#include <Engine/Log.hpp>
#pragma endregion

#pragma region ImGui
#include <Engine/ImGuiRenderer.hpp>
#include <Engine/ImGuiRenderCommand.hpp>
#pragma endregion

#pragma region Renderer
#include <Engine/Renderer.hpp>
#pragma endregion

#pragma region Project
#pragma region Scene
#pragma region Serialize
#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>
#pragma endregion
#include <Engine/Scene.hpp>
#include <Engine/OrthoCamera.hpp>
#include <Engine/PerspectiveCamera.hpp>
#pragma endregion
#include <Engine/Project.hpp>
#pragma endregion

#pragma region Asset
#pragma region Manager
#include <Engine/FileNode.hpp>
#include <Engine/Assets.hpp>
#pragma endregion
#pragma region Color
#include <Engine/Color.hpp>
#pragma endregion
#pragma region Model
#include <Engine/Vertex.hpp>
#pragma endregion
#pragma region ScriptComponent
#pragma region Modules
//#include <Engine/ModelRenderer.hpp>
#pragma endregion
#include <Engine/Angel.hpp>
#include <Engine/PropertyData.hpp>
#include <Engine/EnumValue.hpp>
#pragma endregion
#pragma region Shader
#include <Engine/Shader.hpp>
#pragma endregion
#pragma region Texture
#include <Engine/Texture.hpp>
#pragma endregion
#pragma endregion

#pragma endregion