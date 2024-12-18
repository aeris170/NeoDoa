#include <Engine/Asset.hpp>

#include <Engine/Core.hpp>
#include <Engine/Assets.hpp>
#include <Engine/ProjectDeserializer.hpp>
#include <Engine/SceneSerializer.hpp>
#include <Engine/SceneDeserializer.hpp>
#include <Engine/ComponentDeserializer.hpp>
#include <Engine/SamplerSerializer.hpp>
#include <Engine/SamplerDeserializer.hpp>
#include <Engine/TextureSerializer.hpp>
#include <Engine/TextureDeserializer.hpp>
#include <Engine/ShaderDeserializer.hpp>
#include <Engine/ShaderProgramSerializer.hpp>
#include <Engine/ShaderProgramDeserializer.hpp>
#include <Engine/MaterialSerializer.hpp>
#include <Engine/MaterialDeserializer.hpp>
#include <Engine/FrameBufferSerializer.hpp>
#include <Engine/FrameBufferDeserializer.hpp>

Asset::Asset() noexcept :
    Asset(UUID::Empty(), nullptr) {}

Asset::Asset(const UUID id, FNode* file) noexcept :
    id(id),
    file(file) {}
Asset::~Asset() noexcept { NotifyObservers("destructed"_hs); }
Asset::Asset(Asset&& other) noexcept : ObserverPattern::Observable(std::move(other)),
    id(std::exchange(other.id, UUID::Empty())),
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
    id = std::exchange(other.id, UUID::Empty());
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
    if (IsSampler()) {
        std::string serializedData;
        serializedData = SerializeSampler(DataAs<Sampler>());
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
    if (IsTexture()) {
        EncodedTextureData serializedData;
        serializedData = SerializeTexture(DataAs<Texture>(), ExtToEncoding(file->Extension()));

        std::string stringified;
        stringified.reserve(serializedData.EncodedData.size());
        for (const std::byte b : serializedData.EncodedData) {
            stringified.push_back(static_cast<char>(b));
        }

        file->ModifyContent(std::move(stringified));
        file->DisposeContent();
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
    if (IsMaterial()) {
        std::string serializedData;
        serializedData = SerializeMaterial(DataAs<Material>());
        file->ModifyContent(std::move(serializedData));
        file->DisposeContent();
    }
    if (IsFrameBuffer()) {
        std::string serializedData;
        serializedData = SerializeFrameBuffer(DataAs<FrameBuffer>());
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
                case ComponentCompilerMessageType::Info:
                    infoList.emplace_back(std::move(message));
                    break;
                case ComponentCompilerMessageType::Warning:
                    warningList.emplace_back(std::move(message));
                    break;
                case ComponentCompilerMessageType::Error:
                    errorList.emplace_back(std::move(message));
                    break;
            }
        }
        data = std::move(result.deserializedComponent);
    }
    if (IsSampler()) {
        SamplerDeserializationResult result = DeserializeSampler(*file);
        if (result.erred) {
            for (auto& error : result.errors) {
                errorList.emplace_back(std::move(error));
            }
        }
        data = std::move(result.deserializedSampler);
    }
    if (IsTexture()) {
        TextureDeserializationResult result = DeserializeTexture(*file);
        if (result.erred) {
            for (auto& error : result.errors) {
                errorList.emplace_back(std::move(error));
            }
        }
        data = std::move(result.deserializedTexture);
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
        for (auto& message : result.errors) {
            errorList.emplace_back(std::move(message));
        }
        data = std::move(result.deserializedShader);
    }
    if (IsShaderProgram()) {
        ShaderProgramDeserializationResult result = DeserializeShaderProgram(*file);
        if (result.erred) {
            for (auto& error : result.errors) {
                errorList.emplace_back(std::move(error));
            }
        } else {
            infoList.emplace_back(std::string("Shader Program is complete and ready for use."));
        }
        data = std::move(result.deserializedShaderProgram);
    }
    if (IsMaterial()) {
        MaterialDeserializationResult result = DeserializeMaterial(*file);
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedMaterial);
    }
    if (IsFrameBuffer()) {
        FrameBufferDeserializationResult result = DeserializeFrameBuffer(*file);
        for (auto& error : result.errors) {
            errorList.emplace_back(std::move(error));
        }
        data = std::move(result.deserializedFrameBuffer);
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
bool Asset::IsSampler() const { return Assets::IsSamplerFile(*file); }
bool Asset::IsTexture() const { return Assets::IsTextureFile(*file); }
bool Asset::IsShader() const { return Assets::IsShaderFile(*file); }
bool Asset::IsShaderProgram() const { return Assets::IsShaderProgramFile(*file); }
bool Asset::IsMaterial() const { return Assets::IsMaterialFile(*file); }
bool Asset::IsFrameBuffer() const { return Assets::IsFrameBufferFile(*file); }
bool Asset::IsScript() const { return Assets::IsScriptFile(*file); }
bool Asset::IsModel() const { return Assets::IsModelFile(*file); }

bool Asset::HasInfoMessages() const { return !infoList.empty(); }
const std::vector<std::any>& Asset::InfoMessages() const { return infoList; }

bool Asset::HasWarningMessages() const { return !warningList.empty(); }
const std::vector<std::any>& Asset::WarningMessages() const { return warningList; }

bool Asset::HasErrorMessages() const { return !errorList.empty(); }
const std::vector<std::any>& Asset::ErrorMessages() const { return errorList; }

void Asset::NotifyObservers(ObserverPattern::Notification message) { ObserverPattern::Observable::NotifyObservers(message); }
