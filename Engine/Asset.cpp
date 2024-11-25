#include <Engine/Core.hpp>
#include <Engine/Assets.hpp>

Asset::Asset(const UUID id, Assets& owningManager) noexcept :
    id(id),
    owningManager(owningManager) {}

Asset::~Asset() noexcept {}

Asset::Asset(Asset&& other) noexcept :
    id(std::exchange(other.id, UUID::Empty())),
    owningManager(std::move(other.owningManager)) {}

Asset& Asset::operator=(Asset&& other) noexcept {
    id = std::exchange(other.id, UUID::Empty());
    std::swap(owningManager, other.owningManager);
    DeleteDeserializedData();
    return *this;
}

UUID Asset::ID() const { return id; }
FNode& Asset::File() const { return owningManager.get().GetFileOfAsset(id); }
const AssetData& Asset::Data() const { return owningManager.get().GetDataOfAsset(id); }
uint64_t Asset::Version() const { return owningManager.get().GetVersionOfAsset(id); }

void Asset::Serialize() { owningManager.get().SerializeAsset(id); }
void Asset::Deserialize() { owningManager.get().DeserializeAsset(id); }
void Asset::ForceDeserialize() { owningManager.get().ForceDeserializeAsset(id); }
void Asset::DeleteDeserializedData() { owningManager.get().DeleteDeserializedDataOfAsset(id); }
bool Asset::HasDeserializedData() const { return owningManager.get().AssetHasDeserializedData(id); }

UUID Asset::Instantiate() const { return owningManager.get().InstantiateAsset(id); }

bool Asset::IsScene()               const noexcept { return owningManager.get().IsSceneAsset(id);               }
bool Asset::IsComponentDefinition() const noexcept { return owningManager.get().IsComponentDefinitionAsset(id); }
bool Asset::IsSampler()             const noexcept { return owningManager.get().IsSamplerAsset(id);             }
bool Asset::IsTexture()             const noexcept { return owningManager.get().IsTextureAsset(id);             }
bool Asset::IsShader()              const noexcept { return owningManager.get().IsShaderAsset(id);              }
bool Asset::IsShaderProgram()       const noexcept { return owningManager.get().IsShaderProgramAsset(id);       }
bool Asset::IsMaterial()            const noexcept { return owningManager.get().IsMaterialAsset(id);            }
bool Asset::IsFrameBuffer()         const noexcept { return owningManager.get().IsFrameBufferAsset(id);         }
bool Asset::IsScript()              const noexcept { return owningManager.get().IsScriptAsset(id);              }
bool Asset::IsModel()               const noexcept { return owningManager.get().IsModelAsset(id);               }

bool Asset::HasInfoMessages() const { return owningManager.get().AssetHasInfoMessages(id); }
const std::vector<std::any>& Asset::InfoMessages() const { return owningManager.get().GetInfoMessagesOfAsset(id); }

bool Asset::HasWarningMessages() const { return owningManager.get().AssetHasWarningMessages(id); }
const std::vector<std::any>& Asset::WarningMessages() const { return owningManager.get().GetWarningMessagesOfAsset(id); }

bool Asset::HasErrorMessages() const { return owningManager.get().AssetHasErrorMessages(id); }
const std::vector<std::any>& Asset::ErrorMessages() const { return owningManager.get().GetErrorMessagesOfAsset(id); }
