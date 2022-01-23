#include "SceneDeserializer.hpp"

#include <nameof.hpp>

#include "Scene.hpp"
#include "Core.hpp"
#include "Entity.hpp"
#include "Angel.hpp"
#include "FileNode.hpp"
#include "PropertyData.hpp"
#include "IDComponent.hpp"
#include "TransformComponent.hpp"
#include "ParentComponent.hpp"
#include "ChildComponent.hpp"
#include "ScriptStorageComponent.hpp"
#include "ScriptComponent.hpp"
#include "ScriptComponentData.hpp"

static tinyxml2::XMLElement* findComponentByNameIn(tinyxml2::XMLElement* parent, std::string_view name) {
	for (tinyxml2::XMLElement* comp = parent->FirstChildElement(); comp != nullptr; comp = comp->NextSiblingElement()) {
		if (comp->Attribute("name") == name) {
			return comp;
		}
	}
	return nullptr;
}

static void loopElement(tinyxml2::XMLElement* element, auto lambda) {
	for (tinyxml2::XMLElement* l = element; l != nullptr; l = l->NextSiblingElement()) {
		lambda(l);
	}
}

Scene DeserializeScene(FNode* file) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile(file->_path.c_str());

	tinyxml2::XMLElement* rootNode = doc.RootElement();
	tinyxml2::XMLElement* configNode = rootNode->FirstChildElement("config");
	std::string name = configNode->Attribute("name");

	Scene scene = Scene(file->_name_noext);

	{ // fill config
		tinyxml2::XMLElement* clearColor = configNode->FirstChildElement("clearColor");
		scene.ClearColor = { clearColor->FloatAttribute("r"), clearColor->FloatAttribute("g"), clearColor->FloatAttribute("b") };

		tinyxml2::XMLElement* selectionOutlineColor = configNode->FirstChildElement("selectionOutlineColor");
		scene.SelectionOutlineColor = { selectionOutlineColor->FloatAttribute("r"), selectionOutlineColor->FloatAttribute("g"), selectionOutlineColor->FloatAttribute("b") };

		tinyxml2::XMLElement* camera = configNode->FirstChildElement("camera");
		std::string camType = camera->Attribute("type");
		if (camType == "perspective") {
			scene.SetPerpectiveCamera(camera->FloatAttribute("fov"), camera->FloatAttribute("aspect"), camera->FloatAttribute("near"), camera->FloatAttribute("far"));
		} else if (camType == "ortho") {
			scene.SetOrthoCamera(camera->FloatAttribute("left"), camera->FloatAttribute("right"), camera->FloatAttribute("bottom"), camera->FloatAttribute("top"), camera->FloatAttribute("near"), camera->FloatAttribute("far"));
		}
		tinyxml2::XMLElement* eye = camera->FirstChildElement("eye");
		scene._activeCamera->eye = { eye->FloatAttribute("x"), eye->FloatAttribute("y"), eye->FloatAttribute("z") };

		tinyxml2::XMLElement* forward = camera->FirstChildElement("forward");
		scene._activeCamera->forward = { forward->FloatAttribute("x"), forward->FloatAttribute("y"), forward->FloatAttribute("z") };

		tinyxml2::XMLElement* up = camera->FirstChildElement("up");
		scene._activeCamera->up = { up->FloatAttribute("x"), up->FloatAttribute("y"), up->FloatAttribute("z") };

		tinyxml2::XMLElement* zoom = camera->FirstChildElement("zoom");
		scene._activeCamera->zoom = zoom->FloatAttribute("value");
	}
	{ // parse entities
		tinyxml2::XMLElement* entitiesNode = rootNode->FirstChildElement("entities");
		// all entity elements
		for (tinyxml2::XMLElement* entity = entitiesNode->FirstChildElement(); entity != nullptr; entity = entity->NextSiblingElement()) {
			for (tinyxml2::XMLElement* component = entity->FirstChildElement(); component != nullptr; component = component->NextSiblingElement()) {
				std::string type = component->Name();
				std::string name = component->Attribute("name");
				Entity entt = NULL_ENTT; // the entt we are deserializing for
				ScriptStorageComponent* scripts{ nullptr };
				if (type == "cpp-component") {
					if (name == nameof(IDComponent)) {
						IDComponent id = DeserializeIDComponent(component);
						entt = scene.CreateEntity(id.GetTagRef(), static_cast<uint32_t>(id.GetEntity()));
					} else if (name == nameof(TransformComponent)) {
						TransformComponent transform = DeserializeTransformComponent(component, entt, scene);
						scene.ReplaceComponent<TransformComponent>(transform.GetEntity(), std::move(transform));
					} else if (name == nameof(ParentComponent)) {
						ParentComponent parent = DeserializeParentComponent(component, entt);
						scene.InsertComponent<ParentComponent>(parent.GetEntity(), std::move(parent));
					} else if (name == nameof(ChildComponent)) {
						ChildComponent children = DeserializeChildComponent(component, entt);
						scene.InsertComponent<ChildComponent>(children.GetEntity(), std::move(children));
					}
				} else if (type == "script-component") {
					if (!scene.HasComponent<ScriptStorageComponent>(entt)) {
						scene.EmplaceComponent<ScriptStorageComponent>(entt);
						scripts = &scene.GetComponent<ScriptStorageComponent>(entt);
					}
					std::string name = component->Attribute("name");
					ScriptComponentData data = DeserializeScriptComponentData(component, entt);
					scripts->AttachWithData(name, data);
				}
			}
		}
	}

	return scene;
}

Entity DeserializeEntityID(tinyxml2::XMLElement* property, std::string_view identifierOverride) {
	return static_cast<Entity>(property->UnsignedAttribute(identifierOverride.data(), -1));
}
int DeserializeEnum(tinyxml2::XMLElement* property) {
	return property->IntAttribute("value");
}
int DeserializeInt(tinyxml2::XMLElement* property) {
	return property->IntAttribute("value");
}
float DeserializeFloat(tinyxml2::XMLElement* property) {
	return property->FloatAttribute("value");
}
double DeserializeDouble(tinyxml2::XMLElement* property) {
	return property->DoubleAttribute("value");
}
std::string DeserializeString(tinyxml2::XMLElement* property) {
	return property->Attribute("value");
}
glm::vec2 DeserializeVec2(tinyxml2::XMLElement* property) {
	return {
		property->FloatAttribute("x", 0),
		property->FloatAttribute("y", 0)
	};
}
glm::vec3 DeserializeVec3(tinyxml2::XMLElement* property) {
	return {
		property->FloatAttribute("x", 0),
		property->FloatAttribute("y", 0),
		property->FloatAttribute("z", 0)
	};
}
glm::vec4 DeserializeVec4(tinyxml2::XMLElement* property) {
	return {
		property->FloatAttribute("x", 0),
		property->FloatAttribute("y", 0),
		property->FloatAttribute("z", 0),
		property->FloatAttribute("w", 0)
	};
}
glm::quat DeserializeQuat(tinyxml2::XMLElement* property) {
	return {
		property->FloatAttribute("w", 0),
		property->FloatAttribute("x", 0),
		property->FloatAttribute("y", 0),
		property->FloatAttribute("z", 0)
	};
}
glm::mat2 DeserializeMat2(tinyxml2::XMLElement* property) {
	return {
		property->FloatAttribute("x1", 1), property->FloatAttribute("y1", 0),
		property->FloatAttribute("x2", 0), property->FloatAttribute("y2", 1)
	};
}
glm::mat3 DeserializeMat3(tinyxml2::XMLElement* property) {
	return {
		property->FloatAttribute("x1", 1), property->FloatAttribute("y1", 0), property->FloatAttribute("z1", 0),
		property->FloatAttribute("x2", 0), property->FloatAttribute("y2", 1), property->FloatAttribute("z2", 0),
		property->FloatAttribute("x3", 0), property->FloatAttribute("y3", 0), property->FloatAttribute("z3", 1)
	};
}
glm::mat4 DeserializeMat4(tinyxml2::XMLElement* property) {
	return {
		property->FloatAttribute("x1", 1), property->FloatAttribute("y1", 0), property->FloatAttribute("z1", 0), property->FloatAttribute("w1", 0),
		property->FloatAttribute("x2", 0), property->FloatAttribute("y2", 1), property->FloatAttribute("z2", 0), property->FloatAttribute("w2", 0),
		property->FloatAttribute("x3", 0), property->FloatAttribute("y3", 0), property->FloatAttribute("z3", 1), property->FloatAttribute("w3", 0),
		property->FloatAttribute("x4", 0), property->FloatAttribute("y4", 0), property->FloatAttribute("z4", 0), property->FloatAttribute("w4", 1)
	};
}
IDComponent DeserializeIDComponent(tinyxml2::XMLElement* component) {
	auto entity = DeserializeEntityID(component->FirstChildElement(nameof_c(IDComponent::entity)));
	auto tag = component->FirstChildElement(nameof_c(IDComponent::tag))->Attribute("value");
	return { entity, tag };
}
TransformComponent DeserializeTransformComponent(tinyxml2::XMLElement* component, const Entity entity) {
	auto matrix = DeserializeMat4(component->FirstChildElement(nameof_c(TransformComponent::localMatrix)));

	return { entity, std::move(matrix) };
}
ParentComponent DeserializeParentComponent(tinyxml2::XMLElement* component, const Entity entity) {
	std::vector<Entity> children;
	loopElement(component->FirstChildElement(nameof_c(ParentComponent::children)), [&children](tinyxml2::XMLElement* l) {
		auto child = DeserializeEntityID(l, "value");
		children.push_back(child);
	});

	return { entity, std::move(children) };
}
ChildComponent DeserializeChildComponent(tinyxml2::XMLElement* component, const Entity entity) {
	auto parent = DeserializeEntityID(component->FirstChildElement(nameof_c(ChildComponent::parent)));

	return { entity, parent };
}
ScriptComponentData DeserializeScriptComponentData(tinyxml2::XMLElement* component, const Entity entity) {
	ScriptComponentData data;
	data.name = component->Attribute("name");

	loopElement(component->FirstChildElement(), [&data](tinyxml2::XMLElement* l) {
		std::any value;

		std::string type = l->Attribute("type");
		auto it = GetCore()->_angel->_enums.find(type);
		if (it != GetCore()->_angel->_enums.end())	value = DeserializeEnum(l);
		else if (type == "Entity")					value = DeserializeEntityID(l);
		else if (type == "int")						value = DeserializeInt(l);
		else if (type == "float")					value = DeserializeFloat(l);
		else if (type == "double")					value = DeserializeDouble(l);
		else if (type == "string")					value = DeserializeString(l);
		else if (type == "vec2")					value = DeserializeVec2(l);
		else if (type == "vec3")					value = DeserializeVec3(l);
		else if (type == "vec4")					value = DeserializeVec4(l);
		else if (type == "quat")					value = DeserializeQuat(l);
		else if (type == "mat2")					value = DeserializeMat2(l);
		else if (type == "mat3")					value = DeserializeMat3(l);
		else if (type == "mat4")					value = DeserializeMat4(l);
		else {
			//if (prop.isReference) {
				// TODO
				DOA_LOG_TRACE("prop.isReference ??");
			//}
			//else {
				// TODO
				DOA_LOG_TRACE("!prop.isReference ??");
			//}
		}

		data.data.emplace_back(value);
	});

	return data;
}
