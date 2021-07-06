#include "SceneSerializer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <tinyxml2.h>

#include "Core.hpp"
#include "Angel.hpp"
#include "ScriptComponent.hpp"
#include "Tag.hpp"
#include "Transform.hpp"
#include "ModelRenderer.hpp"
#include "PropertyData.hpp"

std::string SerializeScene(std::weak_ptr<Scene> scene) {
	if (scene.expired()) {
		return std::string();
	}
	auto ptr = scene.lock();
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("scene");
	{
		printer.OpenElement("config");
		{
			printer.PushAttribute("name", ptr->_name.c_str());

			printer.OpenElement("clearColor");
			printer.PushAttribute("r", ptr->ClearColor.r);
			printer.PushAttribute("g", ptr->ClearColor.g);
			printer.PushAttribute("b", ptr->ClearColor.b);
			printer.CloseElement();

			printer.OpenElement("camera");
			{
				bool isPerspective = ptr->_activeCamera == &ptr->_pc;
				if (isPerspective) {
					printer.PushAttribute("type", "perspective");
					printer.PushAttribute("fov", ptr->_pc._fov);
					printer.PushAttribute("aspect", ptr->_pc._aspect);
					printer.PushAttribute("near", ptr->_pc._near);
					printer.PushAttribute("far", ptr->_pc._far);
				} else {
					printer.PushAttribute("type", "ortho");
					printer.PushAttribute("left", ptr->_oc._left);
					printer.PushAttribute("right", ptr->_oc._right);
					printer.PushAttribute("bottom", ptr->_oc._bottom);
					printer.PushAttribute("top", ptr->_oc._top);
					printer.PushAttribute("near", ptr->_oc._near);
					printer.PushAttribute("far", ptr->_oc._far);
				}

				printer.OpenElement("eye");
				printer.PushAttribute("x", ptr->_activeCamera->eye.x);
				printer.PushAttribute("y", ptr->_activeCamera->eye.y);
				printer.PushAttribute("z", ptr->_activeCamera->eye.z);
				printer.CloseElement();

				printer.OpenElement("forward");
				printer.PushAttribute("x", ptr->_activeCamera->forward.x);
				printer.PushAttribute("y", ptr->_activeCamera->forward.y);
				printer.PushAttribute("z", ptr->_activeCamera->forward.z);
				printer.CloseElement();

				printer.OpenElement("up");
				printer.PushAttribute("x", ptr->_activeCamera->up.x);
				printer.PushAttribute("y", ptr->_activeCamera->up.y);
				printer.PushAttribute("z", ptr->_activeCamera->up.z);
				printer.CloseElement();

				printer.OpenElement("zoom");
				printer.PushAttribute("value", ptr->_activeCamera->zoom);
				printer.CloseElement();

			}
			printer.CloseElement(); // camera close

		}
		printer.CloseElement(); // config close

		printer.OpenElement("entities");
		{
			auto& core = GetCore();
			for (auto& entity : ptr->_entities) {
				printer.OpenElement("entity");
				{
					printer.PushAttribute("id", static_cast<int>(entity));
					auto& modules = ptr->Modules(entity);
					for (auto& module : modules._modules) {
						auto* type = module._module->GetObjectType();
						printer.OpenElement("component");
						{
							printer.PushAttribute("name", module._name.c_str());
							if (true) {
								std::vector<PropertyData>& props = module.Properties();
								for (int i = 1; i < props.size(); i++) { // starting from 1, the property at 0 is always the "entity" and it's never shown on the editor
									PropertyData& prop = props[i];
									if (prop.isPrivate || prop.isProtected) { continue; } // don't serialize private or protected variables as they are never visible in editor!
									auto it = core->_angel->_enums.find(prop.typeName);
									printer.OpenElement(prop.name.c_str());
									{
										if (it != core->_angel->_enums.end()) {
											printer.PushAttribute("type", prop.typeName.c_str());
											printer.PushAttribute("value", module.GetAt<int>(i)); // TODO check bug
										} else if (prop.typeName == "int") {
											printer.PushAttribute("type", prop.typeName.c_str());
											printer.PushAttribute("value", module.GetAt<int>(i));
										} else if (prop.typeName == "float") {
											printer.PushAttribute("type", prop.typeName.c_str());
											printer.PushAttribute("value", module.GetAt<float>(i));
										} else if (prop.typeName == "double") {
											printer.PushAttribute("type", prop.typeName.c_str());
											printer.PushAttribute("value", module.GetAt<double>(i));
										} else if (prop.typeName == "string") {
											printer.PushAttribute("type", prop.typeName.c_str());
											printer.PushAttribute("value", module.GetAt<std::string>(i).c_str());
										} else if (prop.typeName == "vec2") {
											printer.PushAttribute("type", prop.typeName.c_str());
											glm::vec2& vec2 = module.GetAt<glm::vec2>(i);
											printer.PushAttribute("x", vec2.x);
											printer.PushAttribute("y", vec2.y);
										} else if (prop.typeName == "vec3") {
											printer.PushAttribute("type", prop.typeName.c_str());
											glm::vec3& vec3 = module.GetAt<glm::vec3>(i);
											printer.PushAttribute("x", vec3.x);
											printer.PushAttribute("y", vec3.y);
											printer.PushAttribute("z", vec3.z);
										} else if (prop.typeName == "vec4") {
											printer.PushAttribute("type", prop.typeName.c_str());
											glm::vec4& vec4 = module.GetAt<glm::vec4>(i);
											printer.PushAttribute("x", vec4.x);
											printer.PushAttribute("y", vec4.y);
											printer.PushAttribute("z", vec4.z);
											printer.PushAttribute("w", vec4.w);
										} else if (prop.typeName == "quat") {
											printer.PushAttribute("type", prop.typeName.c_str());
											glm::quat& quat = module.GetAt<glm::quat>(i);
											printer.PushAttribute("w", quat.w);
											printer.PushAttribute("x", quat.x);
											printer.PushAttribute("y", quat.y);
											printer.PushAttribute("z", quat.z);
										} else if (prop.typeName == "Model") {
											printer.PushAttribute("type", prop.typeName.c_str());
											printer.PushAttribute("value", module.GetAt<Model*>(i)->_name.c_str());
										} else if (prop.typeName == "Shader") {
											printer.PushAttribute("type", prop.typeName.c_str());
											printer.PushAttribute("value", module.GetAt<Shader*>(i)->_name.c_str());
										} else {
											if (!prop.isReference) {
												asIScriptObject*& o = module.GetAt<asIScriptObject*>(i);
												printer.PushAttribute("type", "ptr");
												printer.PushAttribute("refersTo", prop.typeName.c_str());
												if (o == nullptr) {
													printer.PushAttribute("value", "nullptr");
												} else {
													Module m("tmp", o);
													printer.PushAttribute("value", static_cast<int>(m.GetID()));
												}
											} else {
												asIScriptObject& o = module.GetAt<asIScriptObject>(i);
												Module m("tmp", &o);
												printer.PushAttribute("type", "object");
												printer.PushAttribute("refersTo", prop.typeName.c_str());
												EntityID id = m.GetID();
												if (id == NULL_ENTT) {
													printer.PushAttribute("value", "unassigned");
												} else {
													printer.PushAttribute("value", static_cast<int>(id));
												}
											}
										}
									}
									printer.CloseElement(); // prop.name.c_str() end

								}
							}
						}
						printer.CloseElement(); // component close

					}
				}
				printer.CloseElement(); // entity close

			}
		}
		printer.CloseElement(); // entities close

	}
	printer.CloseElement(); // scene close

	return printer.CStr();
}
