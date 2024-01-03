#include <Editor/UserDefinedComponentStorageDeserializer.hpp>

#include <Engine/Core.hpp>
#include <Engine/Project.hpp>

#include <Editor/UserDefinedComponentStorage.hpp>

void DeserializeUserDefinedComponentStorage(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity, const std::string& name) {
    if (!scene.HasComponent<UserDefinedComponentStorage>(entity)) {
        scene.EmplaceComponent<UserDefinedComponentStorage>(entity);
    }

    auto& storage = scene.GetComponent<UserDefinedComponentStorage>(entity);

    UUID id { componentNode.FindAttribute("assetID")->Unsigned64Value() };
    std::vector<ComponentInstance::Field> fields{};

    for (tinyxml2::XMLElement* child = componentNode.FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        std::string typeName{ child->Attribute("type") };
        std::string name{ child->Name() };
        std::any value{};
             if (typeName == "bool")          { value.emplace<bool>    (SceneDeserializer::Helpers::DeserializeBool  (*child)); }
        else if (typeName == "int8")          { value.emplace<int8_t>  (SceneDeserializer::Helpers::DeserializeInt8  (*child)); }
        else if (typeName == "int16")         { value.emplace<int16_t> (SceneDeserializer::Helpers::DeserializeInt16 (*child)); }
        else if (typeName == "int")           { value.emplace<int32_t> (SceneDeserializer::Helpers::DeserializeInt32 (*child)); }
        else if (typeName == "long")          { value.emplace<int64_t> (SceneDeserializer::Helpers::DeserializeInt64 (*child)); }
        else if (typeName == "uint8")         { value.emplace<uint8_t> (SceneDeserializer::Helpers::DeserializeUInt8 (*child)); }
        else if (typeName == "uint16")        { value.emplace<uint16_t>(SceneDeserializer::Helpers::DeserializeUInt16(*child)); }
        else if (typeName == "unsigned int")  { value.emplace<uint32_t>(SceneDeserializer::Helpers::DeserializeUInt32(*child)); }
        else if (typeName == "unsigned long") { value.emplace<uint64_t>(SceneDeserializer::Helpers::DeserializeUInt64(*child)); }
        else if (typeName == "float")         { value.emplace<float_t> (SceneDeserializer::Helpers::DeserializeFloat (*child)); }
        else if (typeName == "double")        { value.emplace<double_t>(SceneDeserializer::Helpers::DeserializeDouble(*child)); }
        fields.emplace_back(typeName, name, value);
    }

    storage.AttachComponentWithData(id, std::move(fields));
}