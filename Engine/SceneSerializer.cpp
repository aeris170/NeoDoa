#include "SceneSerializer.hpp"

#include <entt.hpp>
using namespace entt::literals;

#include <nameof.hpp>

#include "Scene.hpp"
#include "Core.hpp"
#include "Angel.hpp"
#include "ModelRenderer.hpp"
#include "PropertyData.hpp"
#include "IDComponent.hpp"
#include "TransformComponent.hpp"
#include "ParentComponent.hpp"
#include "ChildComponent.hpp"
#include "CameraComponent.hpp"
#include "ScriptStorageComponent.hpp"
#include "ScriptComponent.hpp"

std::string SerializeScene(const Scene& scene) {
	auto& _scene = const_cast<Scene&>(scene);

	tinyxml2::XMLPrinter printer;
	printer.PushComment("WARNING!! This file is not for editing! Don't!");
	printer.OpenElement("scene");
	{
		printer.OpenElement("config");
		{
			printer.PushAttribute("name", _scene.Name.c_str());

			printer.OpenElement("clearColor");
			printer.PushAttribute("r", _scene.ClearColor.r);
			printer.PushAttribute("g", _scene.ClearColor.g);
			printer.PushAttribute("b", _scene.ClearColor.b);
			printer.CloseElement();

			printer.OpenElement("selectionOutlineColor");
			printer.PushAttribute("r", _scene.SelectionOutlineColor.r);
			printer.PushAttribute("g", _scene.SelectionOutlineColor.g);
			printer.PushAttribute("b", _scene.SelectionOutlineColor.b);
			printer.CloseElement();

			printer.OpenElement("cameras");
			{
				printer.OpenElement("activeCamera");
				{
					if (_scene.IsOrtho()) {
						printer.PushAttribute("type", "ortho");
					} else if (_scene.IsPerspective()) {
						printer.PushAttribute("type", "perspective");
					} else {
						assert(false, "no camera?");
					}

					ACamera& activeCamera = _scene.GetActiveCamera();
					printer.OpenElement("eye");
					printer.PushAttribute("x", activeCamera.eye.x);
					printer.PushAttribute("y", activeCamera.eye.y);
					printer.PushAttribute("z", activeCamera.eye.z);
					printer.CloseElement();

					printer.OpenElement("forward");
					printer.PushAttribute("x", activeCamera.forward.x);
					printer.PushAttribute("y", activeCamera.forward.y);
					printer.PushAttribute("z", activeCamera.forward.z);
					printer.CloseElement();

					printer.OpenElement("up");
					printer.PushAttribute("x", activeCamera.up.x);
					printer.PushAttribute("y", activeCamera.up.y);
					printer.PushAttribute("z", activeCamera.up.z);
					printer.CloseElement();

					printer.OpenElement("zoom");
					printer.PushAttribute("value", activeCamera.zoom);
					printer.CloseElement();
				}
				printer.CloseElement();

				printer.OpenElement("orthoCamera");
				{
					OrthoCamera& oc = _scene.GetOrtho();
					printer.PushAttribute("left", oc._left);
					printer.PushAttribute("right", oc._right);
					printer.PushAttribute("bottom", oc._bottom);
					printer.PushAttribute("top", oc._top);
					printer.PushAttribute("near", oc._near);
					printer.PushAttribute("far", oc._far);
				}
				printer.CloseElement();

				printer.OpenElement("perspectiveCamera");
				{
					PerspectiveCamera& pc = _scene.GetPerspective();
					printer.PushAttribute("fov", pc._fov);
					printer.PushAttribute("aspect", pc._aspect);
					printer.PushAttribute("near", pc._near);
					printer.PushAttribute("far", pc._far);
				}
				printer.CloseElement();
			}
			printer.CloseElement(); // cameras close

		}
		printer.CloseElement(); // config close

		printer.OpenElement("entities");
		{
			auto& core = GetCore();
			for (auto& entity : _scene.GetAllEntites()) {
				printer.OpenElement("entity");
				{
					#pragma region IDComponent
					printer.OpenElement("cpp-component");
					{
						printer.PushAttribute("name", nameof_c(IDComponent));
						IDComponent& id = _scene.GetComponent<IDComponent>(entity);
						SerializeIDComponent(printer, id);
					}
					printer.CloseElement();
					#pragma endregion

					#pragma region TransformComponent
					printer.OpenElement("cpp-component");
					{
						printer.PushAttribute("name", nameof_c(TransformComponent));
						TransformComponent& transform = _scene.GetComponent<TransformComponent>(entity);
						SerializeTransformComponent(printer, transform);
					}
					printer.CloseElement();
					#pragma endregion

					#pragma region ParentComponent
					if(_scene.HasComponent<ParentComponent>(entity)) {
						printer.OpenElement("cpp-component");
						{
							printer.PushAttribute("name", nameof_c(ParentComponent));
							ParentComponent& parent = _scene.GetComponent<ParentComponent>(entity);
							SerializeParentComponent(printer, parent);
						}
						printer.CloseElement();
					}
					#pragma endregion

					#pragma region ChildComponent
					if (_scene.HasComponent<ChildComponent>(entity)) {
						printer.OpenElement("cpp-component");
						{
							printer.PushAttribute("name", nameof_c(ChildComponent));
							ChildComponent& child = _scene.GetComponent<ChildComponent>(entity);
							SerializeChildComponent(printer, child);
						}
						printer.CloseElement();
					}
					#pragma endregion

					#pragma region ScriptStorageComponent
					if (_scene.HasComponent<ScriptStorageComponent>(entity)) {
						ScriptStorageComponent& scripts = _scene.GetComponent<ScriptStorageComponent>(entity);
						for (auto& [name, script] : scripts._components) {
							printer.OpenElement("script-component");
							{
								SerializeScriptComponent(printer, script);
							}
							printer.CloseElement();
						}
					}
					#pragma endregion
				}
				printer.CloseElement(); // entity close
			}
		}
		printer.CloseElement(); // entities close
	}
	printer.CloseElement(); // scene close

	return printer.CStr();
}

void SerializeEntityID(tinyxml2::XMLPrinter& printer, Entity entity) {
	printer.PushAttribute("type", "Entity");
	printer.PushAttribute("id", static_cast<uint32_t>(entity));
}
void SerializeEnum(tinyxml2::XMLPrinter& printer, std::string_view name, int value) {
	printer.PushAttribute("type", name.data());
	printer.PushAttribute("value", value);
}
void SerializeInt(tinyxml2::XMLPrinter& printer, int value) {
	printer.PushAttribute("type", "int");
	printer.PushAttribute("value", value);
}
void SerializeFloat(tinyxml2::XMLPrinter& printer, float value) {
	printer.PushAttribute("type", "float");
	printer.PushAttribute("value", value);
}
void SerializeDouble(tinyxml2::XMLPrinter& printer, double value) {
	printer.PushAttribute("type", "double");
	printer.PushAttribute("value", value);
}
void SerializeString(tinyxml2::XMLPrinter& printer, std::string_view value) {
	printer.PushAttribute("type", "string");
	printer.PushAttribute("value", value.data());
}
void SerializeVec2(tinyxml2::XMLPrinter& printer,  const glm::vec2& vec) {
	printer.PushAttribute("type", "vec2");
	printer.PushAttribute("x", vec.x);
	printer.PushAttribute("y", vec.y);
}
void SerializeVec3(tinyxml2::XMLPrinter& printer,  const glm::vec3& vec) {
	printer.PushAttribute("type", "vec3");
	printer.PushAttribute("x", vec.x);
	printer.PushAttribute("y", vec.y);
	printer.PushAttribute("z", vec.z);
}
void SerializeVec4(tinyxml2::XMLPrinter& printer,  const glm::vec4& vec) {
	printer.PushAttribute("type", "vec4");
	printer.PushAttribute("x", vec.x);
	printer.PushAttribute("y", vec.y);
	printer.PushAttribute("z", vec.z);
	printer.PushAttribute("w", vec.w);
}
void SerializeQuat(tinyxml2::XMLPrinter& printer,  const glm::quat& quat) {
	printer.PushAttribute("type", "quat");
	printer.PushAttribute("w", quat.w);
	printer.PushAttribute("x", quat.x);
	printer.PushAttribute("y", quat.y);
	printer.PushAttribute("z", quat.z);
}
void SerializeMat2(tinyxml2::XMLPrinter& printer,  const glm::mat2& mat) {
	printer.PushAttribute("type", "mat2");

	printer.PushAttribute("x1", mat[0][0]);
	printer.PushAttribute("y1", mat[0][1]);

	printer.PushAttribute("x2", mat[1][0]);
	printer.PushAttribute("y2", mat[1][1]);
}
void SerializeMat3(tinyxml2::XMLPrinter& printer,  const glm::mat3& mat) {
	printer.PushAttribute("type", "mat3");

	printer.PushAttribute("x1", mat[0][0]);
	printer.PushAttribute("y1", mat[0][1]);
	printer.PushAttribute("z1", mat[0][2]);

	printer.PushAttribute("x2", mat[1][0]);
	printer.PushAttribute("y2", mat[1][1]);
	printer.PushAttribute("z2", mat[1][2]);

	printer.PushAttribute("x3", mat[2][0]);
	printer.PushAttribute("y3", mat[2][1]);
	printer.PushAttribute("z3", mat[2][2]);

}
void SerializeMat4(tinyxml2::XMLPrinter& printer,  const glm::mat4& mat) {
	printer.PushAttribute("type", "mat4");

	printer.PushAttribute("x1", mat[0][0]);
	printer.PushAttribute("y1", mat[0][1]);
	printer.PushAttribute("z1", mat[0][2]);
	printer.PushAttribute("w1", mat[0][3]);

	printer.PushAttribute("x2", mat[1][0]);
	printer.PushAttribute("y2", mat[1][1]);
	printer.PushAttribute("z2", mat[1][2]);
	printer.PushAttribute("w2", mat[1][3]);

	printer.PushAttribute("x3", mat[2][0]);
	printer.PushAttribute("y3", mat[2][1]);
	printer.PushAttribute("z3", mat[2][2]);
	printer.PushAttribute("w3", mat[2][3]);

	printer.PushAttribute("x4", mat[3][0]);
	printer.PushAttribute("y4", mat[3][1]);
	printer.PushAttribute("z4", mat[3][2]);
	printer.PushAttribute("w4", mat[3][3]);

}
void SerializeIDComponent(tinyxml2::XMLPrinter& printer, const IDComponent& id) {
	auto name = nameof(IDComponent::entity);
	printer.OpenElement(name.data());
	SerializeEntityID(printer, id.GetEntity());
	printer.CloseElement();

	name = nameof(IDComponent::tag);
	printer.OpenElement(name.data());
	printer.PushAttribute("type", "string");
	printer.PushAttribute("value", id.GetTag().data());
	printer.CloseElement();
}
void SerializeTransformComponent(tinyxml2::XMLPrinter& printer, const TransformComponent& transform) {
	auto name = nameof(TransformComponent::localMatrix);
	printer.OpenElement(name.data());
	SerializeMat4(printer, transform.GetLocalMatrix());
	printer.CloseElement();
}
void SerializeParentComponent(tinyxml2::XMLPrinter& printer, const ParentComponent& parent) {
	auto name = nameof(ParentComponent::children);
	printer.OpenElement(name.data());
	{
		printer.PushAttribute("type", "vector");
		printer.OpenElement("values");
		{
			const auto& aux = parent.GetChildren();
			int i = 0;
			for (auto& item : aux) {
				printer.OpenElement("item");
				printer.PushAttribute("index", i);
				printer.PushAttribute("value", static_cast<uint32_t>(item));
				printer.CloseElement();
				i++;
			}
		}
		printer.CloseElement();
	}
	printer.CloseElement();
}
void SerializeChildComponent(tinyxml2::XMLPrinter& printer, const ChildComponent& child) {
	auto name = nameof(ChildComponent::parent);
	printer.OpenElement(name.data());
	SerializeEntityID(printer, child.GetParent());
	printer.CloseElement();
}
void SerializeOrthoCameraComponent(tinyxml2::XMLPrinter& printer, const OrthoCameraComponent& camera) {
	// TODO implement
}
void SerializePerspectiveCameraComponent(tinyxml2::XMLPrinter& printer, const PerspectiveCameraComponent& camera) {
	// TODO implement
}
void SerializeScriptComponent(tinyxml2::XMLPrinter& printer, const ScriptComponent& script) {
	printer.PushAttribute("name", script._name.c_str());
	auto* type = script._underlyingInstance->GetObjectType();

	const std::vector<PropertyData>& props = script.Properties();
	for (int i = 0; i < props.size(); i++) {
		const PropertyData& prop = props[i];
		if (prop.isPrivate || prop.isProtected) { continue; } // don't serialize private or protected variables as they are never visible in editor!
		printer.OpenElement(prop.name.c_str());
		{
			auto it = GetCore()->_angel->_enums.find(prop.typeName);
			if (it != GetCore()->_angel->_enums.end()) SerializeEnum(printer, prop.typeName.c_str(), script.GetAt<int>(i));
			else if (prop.typeName == "Entity")        SerializeEntityID(printer, script.GetAt<Entity>(i));
			else if (prop.typeName == "int")           SerializeInt(printer, script.GetAt<int>(i));
			else if (prop.typeName == "float")         SerializeFloat(printer, script.GetAt<float>(i));
			else if (prop.typeName == "double")        SerializeDouble(printer, script.GetAt<double>(i));
			else if (prop.typeName == "string")        SerializeString(printer, script.GetAt<std::string>(i).c_str());
			else if (prop.typeName == "vec2")          SerializeVec2(printer, script.GetAt<glm::vec2>(i));
			else if (prop.typeName == "vec3")          SerializeVec3(printer, script.GetAt<glm::vec3>(i));
			else if (prop.typeName == "vec4")          SerializeVec4(printer, script.GetAt<glm::vec4>(i));
			else if (prop.typeName == "quat")          SerializeQuat(printer, script.GetAt<glm::quat>(i));
			else if (prop.typeName == "mat2")          SerializeMat2(printer, script.GetAt<glm::mat2>(i));
			else if (prop.typeName == "mat3")          SerializeMat3(printer, script.GetAt<glm::mat3>(i));
			else if (prop.typeName == "mat4")          SerializeMat4(printer, script.GetAt<glm::mat4>(i));
			else {
				if (prop.isReference) {
					// TODO
					DOA_LOG_TRACE("prop.isReference");
				}
				else {
					// TODO
					DOA_LOG_TRACE("!prop.isReference");
				}
			}
		}
		printer.CloseElement(); // prop.name.c_str() end
	}
}
