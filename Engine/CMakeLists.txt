cmake_minimum_required(VERSION 3.26.4)

project(Engine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(Engine STATIC)
target_include_directories(Engine PUBLIC ..)

target_link_libraries(Engine PUBLIC Utility)
target_link_libraries(Engine PUBLIC angelscript_addons_impl)
target_link_libraries(Engine PUBLIC stb_impl)

find_package(assimp CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(eventpp CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc dt i18n)

target_link_libraries(Engine PUBLIC assimp::assimp)
target_link_libraries(Engine PUBLIC imgui::imgui)
target_link_libraries(Engine PUBLIC EnTT::EnTT)
target_link_libraries(Engine PUBLIC eventpp::eventpp)
target_link_libraries(Engine PUBLIC GLEW::GLEW)
target_link_libraries(Engine PUBLIC glfw)
target_link_libraries(Engine PUBLIC glm::glm)
target_link_libraries(Engine PUBLIC tinyxml2::tinyxml2)
target_link_libraries(Engine PUBLIC IconFontCppHeaders)
target_link_libraries(Engine PUBLIC ICU::uc ICU::dt ICU::i18n)

set(GROUP_LIST
    "NeoDoa.hpp"
	
    "TypedefsAndConstants.hpp"
	
    "Asset/Asset.cpp"
    "Asset/Asset.hpp"
    "Asset/Assets.cpp"
    "Asset/Assets.hpp"
    "Asset/UUID.cpp"
    "Asset/UUID.hpp"
    
    "Asset/Color/Color.hpp"
    "Asset/Component/Component.cpp"
    "Asset/Component/Component.hpp"
    "Asset/Component/Serialize/ComponentDeserializer.cpp"
    "Asset/Component/Serialize/ComponentDeserializer.hpp"
    "Asset/Manager/FileNode.cpp"
    "Asset/Manager/FileNode.hpp"
    "Asset/Material/Material.cpp"
    "Asset/Material/Material.hpp"
    "Asset/Material/MaterialSerializer.cpp"
    "Asset/Material/MaterialSerializer.hpp"
    "Asset/Material/MaterialDeserializer.cpp"
    "Asset/Material/MaterialDeserializer.hpp"
    "Asset/Model/Avatar.hpp"
    "Asset/Model/Model.cpp"
    "Asset/Model/Model.hpp"
    "Asset/Sampler/Sampler.cpp"
    "Asset/Sampler/Sampler.hpp"
    "Asset/Script/Angel.cpp"
    "Asset/Script/Angel.hpp"
    "Asset/Script/EnumValue.hpp"
    "Asset/Script/PropertyData.hpp"
    "Asset/Shader/Shader.cpp"
    "Asset/Shader/Shader.hpp"
    "Asset/Shader/Serialize/ShaderDeserializer.cpp"
    "Asset/Shader/Serialize/ShaderDeserializer.hpp"
    "Asset/Shader/Serialize/ShaderProgramSerializer.cpp"
    "Asset/Shader/Serialize/ShaderProgramSerializer.hpp"
    "Asset/Shader/Serialize/ShaderProgramDeserializer.cpp"
    "Asset/Shader/Serialize/ShaderProgramDeserializer.hpp"
    "Asset/Texture/Texture.cpp"
    "Asset/Texture/Texture.hpp"
    "Asset/Texture/Serialize/TextureSerializer.cpp"
    "Asset/Texture/Serialize/TextureSerializer.hpp"
    "Asset/Texture/Serialize/TextureDeserializer.cpp"
    "Asset/Texture/Serialize/TextureDeserializer.hpp"
	
    "Core/Core.cpp"
    "Core/Core.hpp"
    "Core/Input.cpp"
    "Core/Input.hpp"
    "Core/Window.cpp"
    "Core/Window.hpp"
    "Core/TypeSystem.hpp"
	
    "Event/Event.hpp"
	
    "OpenGL/OpenGLCommon.hpp"
    "OpenGL/Buffer.cpp"
    "OpenGL/Buffer.hpp"
    "OpenGL/FrameBuffer.cpp"
    "OpenGL/FrameBuffer.hpp"
    "OpenGL/VertexArray.cpp"
    "OpenGL/VertexArray.hpp"
    "OpenGL/VertexAttribLayout.cpp"
    "OpenGL/VertexAttribLayout.hpp"
	
    "ECS/Registry.hpp"
    "ECS/Components/BehaviourComponent.cpp"
    "ECS/Components/BehaviourComponent.hpp"
    "ECS/Components/CameraComponent.cpp"
    "ECS/Components/CameraComponent.hpp"
    "ECS/Components/ChildComponent.cpp"
    "ECS/Components/ChildComponent.hpp"
    "ECS/Components/IDComponent.cpp"
    "ECS/Components/IDComponent.hpp"
    "ECS/Components/ParentComponent.cpp"
    "ECS/Components/ParentComponent.hpp"
    "ECS/Components/TransformComponent.cpp"
    "ECS/Components/TransformComponent.hpp"
    "ECS/Entity/Entity.cpp"
    "ECS/Entity/Entity.hpp"
    "ECS/Systems/System.cpp"
    "ECS/Systems/System.hpp"
	
    "ImGui/FontAwesome.hpp"
    "ImGui/ImGuiRenderCommand.hpp"
    "ImGui/ImGuiRenderer.cpp"
    "ImGui/ImGuiRenderer.hpp"
	
    "Log/Log.cpp"
    "Log/Log.hpp"
	
    "Misc/AABB.cpp"
    "Misc/AABB.hpp"
    "Misc/Vertex.hpp"
    "Misc/Region.hpp"
    "Misc/Resolution.hpp"
    "Misc/OstreamImpls.cpp"
    "Misc/AssimpGLMConvert.hpp"
	
    "Project/Project.cpp"
    "Project/Project.hpp"
    "Project/Serialize/ProjectSerializer.cpp"
    "Project/Serialize/ProjectSerializer.hpp"
    "Project/Serialize/ProjectDeserializer.cpp"
    "Project/Serialize/ProjectDeserializer.hpp"
    "Project/Scene/ACamera.hpp"
    "Project/Scene/OrthoCamera.cpp"
    "Project/Scene/OrthoCamera.hpp"
    "Project/Scene/PerspectiveCamera.cpp"
    "Project/Scene/PerspectiveCamera.hpp"
    "Project/Scene/Scene.cpp"
    "Project/Scene/Scene.hpp"
    "Project/Scene/Serialize/SceneSerializer.cpp"
    "Project/Scene/Serialize/SceneSerializer.hpp"
    "Project/Scene/Serialize/SceneDeserializer.cpp"
    "Project/Scene/Serialize/SceneDeserializer.hpp"
	
    "Renderer/Renderer.cpp"
    "Renderer/Renderer.hpp"
    "Renderer/OutlineRenderer.cpp"
    "Renderer/OutlineRenderer.hpp"
)
foreach(source IN LISTS GROUP_LIST)
    get_filename_component(source_path "${source}" PATH)
    get_filename_component(source_name "${source}" NAME)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source_name}")
    target_sources(Engine PRIVATE "${source_name}")
endforeach()

if(MSVC)
 target_compile_options(Engine PRIVATE "/MP")
endif()

copy_folder_post_build(Fonts)
copy_folder_post_build(Images)
copy_folder_post_build(Models)
copy_folder_post_build(Shaders)
