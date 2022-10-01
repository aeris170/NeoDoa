#include "Asset.hpp"

#include "Assets.hpp"
#include "ProjectDeserializer.hpp"
#include "SceneDeserializer.hpp"
#include "SceneSerializer.hpp"

Asset::Asset() noexcept :
	Asset(UUID::Empty(), nullptr) {}

Asset::Asset(const UUID id, FNode* file) noexcept :
	id(id),
	file(file) {}

Asset::Asset(Asset&& other) noexcept :
	id(std::move(other.id)),
	file(std::exchange(other.file, nullptr)) {
	DeleteDeserializedData();
}

Asset& Asset::operator=(Asset&& other) noexcept {
	id = std::move(other.id);
	file = std::exchange(other.file, nullptr);
	DeleteDeserializedData();
	return *this;
}

UUID Asset::ID() const { return id; }
FNode* Asset::File() const { return file; }
const AssetData& Asset::Data() const { return data; }

void Asset::Serialize() {
	std::string serializedData;

	if (IsScene()) {
		serializedData = SerializeScene(DataAs<Scene>());
	}
	/*
	* TODO others
	*/

	tinyxml2::XMLDocument doc;
	doc.Parse(serializedData.c_str());
	doc.SaveFile(file->AbsolutePath().string().c_str());
}

void Asset::Deserialize() {
	if (!std::get_if<std::monostate>(&data)) { return; } // already deserialized

	file->ReadContent();

	if (IsScene()) {
		data = DeserializeScene(file->content);
	}
	/*
	* TODO others
	*/

	file->DisposeContent();
}
void Asset::ForceDeserialize() {
	DeleteDeserializedData();
	Deserialize();
}
void Asset::DeleteDeserializedData() { data = std::monostate{}; }

UUID Asset::Instantiate() const {
	return UUID();
}

bool Asset::IsScene() const { return Assets::IsSceneFile(file); }
bool Asset::IsScript() const { return Assets::IsScriptFile(file); }
bool Asset::IsTexture() const { return Assets::IsTextureFile(file); }
bool Asset::IsModel() const { return Assets::IsModelFile(file); }
bool Asset::IsMaterial() const { return Assets::IsMaterialFile(file); }
bool Asset::IsShader() const { return Assets::IsShaderFile(file); }
