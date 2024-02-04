#include <Engine/Asset.hpp>

#include <Engine/Assets.hpp>
#include <Engine/ProjectDeserializer.hpp>
#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>
#include <Engine/ComponentDeserializer.hpp>
#include <Engine/ShaderDeserializer.hpp>
#include <Engine/ShaderProgramSerializer.hpp>
#include <Engine/ShaderProgramDeserializer.hpp>
#include <Engine/TextureSerializer.hpp>
#include <Engine/TextureDeserializer.hpp>

Asset::Asset() noexcept :
    Asset(UUID::Empty(), nullptr) {}

Asset::Asset(const UUID id, FNode* file) noexcept :
    id(id),
    file(file) {}
Asset::~Asset() noexcept { NotifyObservers("destructed"_hs); }
Asset::Asset(Asset&& other) noexcept : ObserverPattern::Observable(std::move(other)),
    id(std::move(other.id)),
    file(std::exchange(other.file, nullptr)),
    data(std::move(other.data)),
    version(std::exchange(other.version, 0)),
    infoList(std::move(other.infoList)),
    warningList(std::move(other.warningList)),
    errorList(std::move(other.errorList)) {
    NotifyObservers("moved"_hs);
}
Asset& Asset::operator=(Asset&& other) noexcept {
    ObserverPattern::Observable::operator=(std::move(other));
    id = std::move(other.id);
    file = std::exchange(other.file, nullptr);
    DeleteDeserializedData();
    data.swap(other.data);
    version = std::exchange(other.version, 0);
    infoList = std::move(other.infoList);
    warningList = std::move(other.warningList);
    errorList = std::move(other.errorList);
    NotifyObservers("moved"_hs);
    return *this;
}

UUID Asset::ID() const { return id; }
FNode& Asset::File() const { return *file; }
const AssetData& Asset::Data() const { return data; }
uint64_t Asset::Version() const { return version; }

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
    if (IsShaderProgram()) {
        std::string serializedData;
        serializedData = SerializeShaderProgram(DataAs<ShaderProgram>());
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
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
    if (IsShader()) {
        ShaderDeserializationResult result;
        if (Assets::IsVertexShaderFile(*file)) {
            result = DeserializeVertexShader(*file);
        }
        if (Assets::IsTessellationControlShaderFile(*file)) {
            result = DeserializeTessellationControlShader(*file);
        }
        if (Assets::IsTessellationEvaluationShaderFile(*file)) {
            result = DeserializeTessellationEvaluationShader(*file);
        }
        if (Assets::IsGeometryShaderFile(*file)) {
            result = DeserializeGeometryShader(*file);
        }
        if (Assets::IsFragmentShaderFile(*file)) {
            result = DeserializeFragmentShader(*file);
        }
        if (Assets::IsComputeShaderFile(*file)) {
            result = DeserializeComputeShader(*file);
        }
        for (auto& message : result.messages) {
            switch (message.messageType) {
            case ShaderCompilerMessageType::INFO:
                infoList.emplace_back(std::move(message));
                break;
            case ShaderCompilerMessageType::WARNING:
                warningList.emplace_back(std::move(message));
                break;
            case ShaderCompilerMessageType::ERROR:
                errorList.emplace_back(std::move(message));
                break;
            }
        }
        data = std::move(result.deserializedShader);
    }
    if (IsShaderProgram()) {
        ShaderProgramDeserializationResult result = DeserializeShaderProgram(*file);
        if (result.erred) {
            for (auto& message : result.errors) {
                errorList.emplace_back(std::move(message));
            }
        } else {
            infoList.emplace_back(std::string("Shader Program is complete and ready for use."));
        }
        data = std::move(result.deserializedShaderProgram);
    }
    if (IsTexture()) {
        data = DeserializeTexture(*file);
    }
    /*
    * TODO others
    */
    version++;
    NotifyObservers("deserialized"_hs);
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
    version++;
    NotifyObservers("data_deleted"_hs);
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
bool Asset::IsShaderProgram() const { return Assets::IsShaderProgramFile(*file); }

bool Asset::HasInfoMessages() const { return !infoList.empty(); }
const std::vector<std::any>& Asset::InfoMessages() const { return infoList; }

bool Asset::HasWarningMessages() const { return !warningList.empty(); }
const std::vector<std::any>& Asset::WarningMessages() const { return warningList; }

bool Asset::HasErrorMessages() const { return !errorList.empty(); }
const std::vector<std::any>& Asset::ErrorMessages() const { return errorList; }

void Asset::NotifyObservers(ObserverPattern::Notification message) { ObserverPattern::Observable::NotifyObservers(message); }
