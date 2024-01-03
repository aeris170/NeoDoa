#include <Editor/UserDefinedComponentStorageSerializer.hpp>

#include <Editor/UserDefinedComponentStorage.hpp>

void SerializeUserDefinedComponentStorage(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity) {
    if (!scene.HasComponent<UserDefinedComponentStorage>(entity)) { return; }

    auto& storage = scene.GetComponent<UserDefinedComponentStorage>(entity);
    for (auto& [name, instance] : storage.Components()) {

        printer.OpenElement("user-component");
        {
            printer.PushAttribute("name", name.c_str());
            printer.PushAttribute("assetID", instance.ComponentAssetID());
            for (auto& field : instance.MemberValues()) {
                printer.OpenElement(field.Name().c_str());
                {
                    std::string typeName{ field.TypeName() };
                         if (typeName == "bool")          { SceneSerializer::Helpers::SerializeBool   (printer, field.As<bool>());     }
                    else if (typeName == "int8")          { SceneSerializer::Helpers::SerializeInt8   (printer, field.As<int8_t>());   }
                    else if (typeName == "int16")         { SceneSerializer::Helpers::SerializeInt16  (printer, field.As<int16_t>());  }
                    else if (typeName == "int")           { SceneSerializer::Helpers::SerializeInt32  (printer, field.As<int32_t>());  }
                    else if (typeName == "long")          { SceneSerializer::Helpers::SerializeInt64  (printer, field.As<int64_t>());  }
                    else if (typeName == "uint8")         { SceneSerializer::Helpers::SerializeUInt8  (printer, field.As<uint8_t>());  }
                    else if (typeName == "uint16")        { SceneSerializer::Helpers::SerializeUInt16 (printer, field.As<uint16_t>()); }
                    else if (typeName == "unsigned int")  { SceneSerializer::Helpers::SerializeUInt32 (printer, field.As<uint32_t>()); }
                    else if (typeName == "unsigned long") { SceneSerializer::Helpers::SerializeUInt64 (printer, field.As<uint64_t>()); }
                    else if (typeName == "float")         { SceneSerializer::Helpers::SerializeFloat  (printer, field.As<float_t>());  }
                    else if (typeName == "double")        { SceneSerializer::Helpers::SerializeDouble (printer, field.As<double_t>()); }
                }
                printer.CloseElement();
            }
        }
        printer.CloseElement();
    }
}