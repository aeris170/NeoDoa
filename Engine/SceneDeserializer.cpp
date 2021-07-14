#include "SceneDeserializer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <tinyxml2.h>

#include "Core.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Angel.hpp"
#include "ScriptComponent.hpp"
#include "Tag.hpp"
#include "Transform.hpp"
#include "FileNode.hpp"
#include "PropertyData.hpp"
#include "FileNode.hpp"

static tinyxml2::XMLElement* findComponentByNameIn(tinyxml2::XMLElement* parent, std::string_view name) {
	for (tinyxml2::XMLElement* comp = parent->FirstChildElement(); comp != nullptr; comp = comp->NextSiblingElement()) {
		if (comp->Attribute("name") == name) {
			return comp;
		}
	}
	return nullptr;
}

std::shared_ptr<Scene> DeserializeScene(FNode* file) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile(file->_path.c_str());

	tinyxml2::XMLElement* rootNode = doc.RootElement();
	tinyxml2::XMLElement* configNode = rootNode->FirstChildElement("config");
	std::string name = configNode->Attribute("name");

	std::weak_ptr<Scene> ptr = FindSceneByName(name);
	if (!ptr.expired()) {
		return ptr.lock();
	}

	ptr = CreateScene(name);
	std::shared_ptr<Scene> scene = ptr.lock();

	{ // fill config
		tinyxml2::XMLElement* clearColor = configNode->FirstChildElement("clearColor");
		scene->ClearColor = { clearColor->FloatAttribute("r"), clearColor->FloatAttribute("g"), clearColor->FloatAttribute("b") };

		tinyxml2::XMLElement* camera = configNode->FirstChildElement("camera");
		std::string camType = camera->Attribute("type");
		if (camType == "perspective") {
			scene->SetPerpectiveCamera(camera->FloatAttribute("fov"), camera->FloatAttribute("aspect"), camera->FloatAttribute("near"), camera->FloatAttribute("far"));
		} else if (camType == "ortho") {
			scene->SetOrthoCamera(camera->FloatAttribute("left"), camera->FloatAttribute("right"), camera->FloatAttribute("bottom"), camera->FloatAttribute("top"), camera->FloatAttribute("near"), camera->FloatAttribute("far"));
		}
		tinyxml2::XMLElement* eye = camera->FirstChildElement("eye");
		scene->_activeCamera->eye = { eye->FloatAttribute("x"), eye->FloatAttribute("y"), eye->FloatAttribute("z") };

		tinyxml2::XMLElement* forward = camera->FirstChildElement("forward");
		scene->_activeCamera->forward = { forward->FloatAttribute("x"), forward->FloatAttribute("y"), forward->FloatAttribute("z") };

		tinyxml2::XMLElement* up = camera->FirstChildElement("up");
		scene->_activeCamera->up = { up->FloatAttribute("x"), up->FloatAttribute("y"), up->FloatAttribute("z") };

		tinyxml2::XMLElement* zoom = camera->FirstChildElement("zoom");
		scene->_activeCamera->zoom = zoom->FloatAttribute("value");
	}
	{ // parse entities
		tinyxml2::XMLElement* entitiesNode = rootNode->FirstChildElement("entities");
		// all entity elements
		for (tinyxml2::XMLElement* entity = entitiesNode->FirstChildElement(); entity != nullptr; entity = entity->NextSiblingElement()) {
			int id = entity->IntAttribute("id");
			auto* tag = findComponentByNameIn(entity, "Tag");
			std::string name = tag->FirstChildElement("label")->Attribute("value");
			EntityID entt = scene->CreateEntity(name.c_str(), id);

			auto* transform = findComponentByNameIn(entity, "Transform");
			auto& scr = scene->Modules(entt);
			auto& tr = scr["Transform"].As<Transform>();

			auto* elem = transform->FirstChildElement();
			tr.Translation() = { elem->FloatAttribute("x"), elem->FloatAttribute("y"), elem->FloatAttribute("z") };
			elem = elem->NextSiblingElement();
			tr.Rotation() = { elem->FloatAttribute("w"), elem->FloatAttribute("x"), elem->FloatAttribute("y"), elem->FloatAttribute("z") };
			elem = elem->NextSiblingElement();
			tr.Scale() = { elem->FloatAttribute("x"), elem->FloatAttribute("y"), elem->FloatAttribute("z") };

			// Loop all componenets and add missing ones.
			for (tinyxml2::XMLElement* component = transform->NextSiblingElement(); component != nullptr; component = component->NextSiblingElement()) {
				std::string name = component->Attribute("name");
				auto opt_module = scr.TryGet(name);
				if (!opt_module.has_value()) {
					scr.Attach(name);
				}
			}

			// Fill the information the components require.
			for (tinyxml2::XMLElement* component = transform->NextSiblingElement(); component != nullptr; component = component->NextSiblingElement()) {
				std::string name = component->Attribute("name");
				auto& module = scr[name];
				int i = 1; // start i from 1 because 0 is the entity id and should not be modified (from anywhere outside CreateEntity func)
				auto& core = GetCore();
				for (tinyxml2::XMLElement* property = component->FirstChildElement(); property != nullptr; property = property->NextSiblingElement(), i++) {
					std::string typeName = property->Attribute("type");
					void* address = module.GetAddressOfPropertyAt(i);
					auto it = core->_angel->_enums.find(typeName);
					if (it != core->_angel->_enums.end()) {
						int* ptr = static_cast<int*>(address);
						*ptr = property->IntAttribute("value");
					} else if (typeName == "int") {
						int* ptr = static_cast<int*>(address);
						*ptr = property->IntAttribute("value");
					} else if (typeName == "float") {
						float* ptr = static_cast<float*>(address);
						*ptr = property->FloatAttribute("value");
					} else if (typeName == "double") {
						double* ptr = static_cast<double*>(address);
						*ptr = property->DoubleAttribute("value");
					} else if (typeName == "string") {
						std::string* ptr = static_cast<std::string*>(address);
						*ptr = property->Attribute("value");
					} else if (typeName == "vec2") {
						glm::vec2* ptr = static_cast<glm::vec2*>(address);
						*ptr = { property->FloatAttribute("x"), property->FloatAttribute("y") };
					} else if (typeName == "vec3") {
						glm::vec3* ptr = static_cast<glm::vec3*>(address);
						*ptr = { property->FloatAttribute("x"), property->FloatAttribute("y"), property->FloatAttribute("z") };
					} else if (typeName == "vec4") {
						glm::vec4* ptr = static_cast<glm::vec4*>(address);
						*ptr = { property->FloatAttribute("x"), property->FloatAttribute("y"), property->FloatAttribute("z"), property->FloatAttribute("w") };
					} else if (typeName == "quat") {
						glm::quat* ptr = static_cast<glm::quat*>(address);
						*ptr = { property->FloatAttribute("w"), property->FloatAttribute("x"), property->FloatAttribute("y"), property->FloatAttribute("z") };
					} else if (typeName == "Model") {
						Model** ptr = static_cast<Model**>(address);
						auto opt_model = FindModel(property->Attribute("value"));
						if (opt_model.has_value()) {
							*ptr = opt_model.value().lock().get();
						} else {
							*ptr = nullptr;
						}
					} else if (typeName == "Shader") {
						Shader** ptr = static_cast<Shader**>(address);
						auto opt_model = FindShader(property->Attribute("value"));
						if (opt_model.has_value()) {
							*ptr = opt_model.value().lock().get();
						} else {
							*ptr = nullptr;
						}
					} else if (typeName == "ptr") {
						asIScriptObject** ptr = static_cast<asIScriptObject**>(address);
						std::string componentNeeded = property->Attribute("refersTo");
						std::string componentOwner = property->Attribute("value");
						if (componentOwner != "nullptr") {
							int componentOwnerID = property->IntAttribute("value");
							*ptr = scene->Modules(static_cast<EntityID>(componentOwnerID))[componentNeeded];
							(*ptr)->AddRef();
						} else {
							*ptr = nullptr;
						}
					} else if (typeName == "object") {
						asIScriptObject* ptr = static_cast<asIScriptObject*>(address);
						std::string componentOwner = property->Attribute("value");
						Module m("tmp", ptr);
						if (componentOwner != "unassigned") {
							int componentOwnerID = property->IntAttribute("value");
							m.SetID(static_cast<EntityID>(componentOwnerID));
						} else {
							m.SetID(NULL_ENTT);
						}
					}
				}
			}
		}
	}

	return scene;
}
