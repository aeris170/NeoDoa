#include "EditorMetaInitializer.hpp"

#include <SceneSerializer.hpp>
#include <SceneDeserializer.hpp>

#include "UserDefinedComponentStorageSerializer.hpp"
#include "UserDefinedComponentStorageDeserializer.hpp"

void InitializeEditorMeta() {
    SceneSerializer::Entities::SerializeUserDefinedComponents = SerializeUserDefinedComponentStorage;
    SceneDeserializer::Entities::DeserializeUserDefinedComponents = DeserializeUserDefinedComponentStorage;
}