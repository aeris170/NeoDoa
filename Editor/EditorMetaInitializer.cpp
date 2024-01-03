#include <Editor/EditorMetaInitializer.hpp>

#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>

#include <Editor/UserDefinedComponentStorageSerializer.hpp>
#include <Editor/UserDefinedComponentStorageDeserializer.hpp>

void InitializeEditorMeta() {
    SceneSerializer::Entities::SerializeUserDefinedComponents = SerializeUserDefinedComponentStorage;
    SceneDeserializer::Entities::DeserializeUserDefinedComponents = DeserializeUserDefinedComponentStorage;
}