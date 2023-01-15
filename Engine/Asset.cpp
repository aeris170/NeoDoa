#include "Asset.hpp"

#include "Assets.hpp"
#include "ProjectDeserializer.hpp"
#include "SceneDeserializer.hpp"
#include "SceneSerializer.hpp"
#include "ComponentDeserializer.hpp"
#include "TextureDeserializer.hpp"
#include "TextureSerializer.hpp"

Asset::Asset() noexcept :
    Asset(UUID::Empty(), nullptr) {}

Asset::Asset(const UUID id, FNode* file) noexcept :
    id(id),
    file(file) {}

Asset::Asset(Asset&& other) noexcept :
    id(std::move(other.id)),
    file(std::exchange(other.file, nullptr)),
    data(std::move(other.data)),
    infoList(std::move(other.infoList)),
    warningList(std::move(other.warningList)),
    errorList(std::move(other.errorList)) {}

Asset& Asset::operator=(Asset&& other) noexcept {
    id = std::move(other.id);
    file = std::exchange(other.file, nullptr);
    DeleteDeserializedData();
    data.swap(other.data);
    infoList = std::move(other.infoList);
    warningList = std::move(other.warningList);
    errorList = std::move(other.errorList);
    return *this;
}

UUID Asset::ID() const { return id; }
FNode& Asset::File() const { return *file; }
const AssetData& Asset::Data() const { return data; }

void Asset::Serialize() {
    if (IsScene()) {
        std::string serializedData;
        serializedData = SerializeScene(DataAs<Scene>());
        tinyxml2::XMLDocument doc;
        doc.Parse(serializedData.c_str());
        doc.SaveFile(file->AbsolutePath().string().c_str());
        return;
    }
    if (IsTexture()) {
        SerializeTexture(DataAs<Texture>());
        // TODO WRITE TO FILE
    }
    /*
    * TODO others
    */
}

void Asset::Deserialize() {
    if (HasDeserializedData()) { return; }

    if (IsScene()) {
        data = DeserializeScene(*file);
    }
    if (IsComponentDefinition()) {
        auto result = DeserializeComponent(*file);
        for (auto& message : result.messages) {
            switch (message.messageType) {
                case ComponentCompilerMessageType::INFO:
                    infoList.emplace_back(std::move(message));
                    break;
                case ComponentCompilerMessageType::WARNING:
                    warningList.emplace_back(std::move(message));
                    break;
                case ComponentCompilerMessageType::ERROR:
                    errorList.emplace_back(std::move(message));
                    break;
            }
        }
        data = std::move(result.deserializedComponent);
    }
    if (IsTexture()) {
        data = DeserializeTexture(*file);
    }
    /*
    * TODO others
    */
}
void Asset::ForceDeserialize() {
    DeleteDeserializedData();
    Deserialize();
}
void Asset::DeleteDeserializedData() {
    data = std::monostate{};
    infoList.clear();
    warningList.clear();
    errorList.clear();
}
bool Asset::HasDeserializedData() const { return !std::holds_alternative<std::monostate>(data); }

UUID Asset::Instantiate() const {
    return UUID();
}

bool Asset::IsScene() const { return Assets::IsSceneFile(*file); }
bool Asset::IsComponentDefinition() const { return Assets::IsComponentDefinitionFile(*file); }
bool Asset::IsScript() const { return Assets::IsScriptFile(*file); }
bool Asset::IsTexture() const { return Assets::IsTextureFile(*file); }
bool Asset::IsModel() const { return Assets::IsModelFile(*file); }
bool Asset::IsMaterial() const { return Assets::IsMaterialFile(*file); }
bool Asset::IsShader() const { return Assets::IsShaderFile(*file); }

bool Asset::HasInfoMessages() const { return infoList.size() > 0; }
const std::vector<std::any>& Asset::InfoMessages() const { return infoList; }

bool Asset::HasWarningMessages() const { return warningList.size() > 0; }
const std::vector<std::any>& Asset::WarningMessages() const { return warningList; }

bool Asset::HasErrorMessages() const { return errorList.size() > 0; }
const std::vector<std::any>& Asset::ErrorMessages() const { return errorList; }