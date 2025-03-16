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
const Assets& Asset::OwningManager() const { return owningManager; }
FNode& Asset::File() const { return owningManager.get().GetFileOfAsset(id); }
const AssetData& Asset::Data() const { return owningManager.get().GetDataOfAsset(id); }
SubAssetList Asset::SubAssets() { return owningManager.get().GetSubAssetsOfAsset(id); }
const SubAssetList Asset::SubAssets() const { return owningManager.get().GetSubAssetsOfAsset(id); }
uint64_t Asset::Version() const { return owningManager.get().GetVersionOfAsset(id); }
std::optional<std::string_view> Asset::TryGetName() const { return owningManager.get().TryGetNameOfAsset(id); };
HashedString Asset::TypeName() const { return owningManager.get().GetTypeNameOfAsset(id); };

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
bool Asset::IsMesh()                const noexcept { return owningManager.get().IsMeshAsset(id);                }
bool Asset::IsModel()               const noexcept { return owningManager.get().IsModelAsset(id);               }
bool Asset::IsScript()              const noexcept { return owningManager.get().IsScriptAsset(id);              }

bool Asset::IsSubAsset() const noexcept { return owningManager.get().IsSubAsset(id); }

bool Asset::HasInfoMessages() const { return owningManager.get().AssetHasInfoMessages(id); }
const std::vector<std::any>& Asset::InfoMessages() const { return owningManager.get().GetInfoMessagesOfAsset(id); }

bool Asset::HasWarningMessages() const { return owningManager.get().AssetHasWarningMessages(id); }
const std::vector<std::any>& Asset::WarningMessages() const { return owningManager.get().GetWarningMessagesOfAsset(id); }

bool Asset::HasErrorMessages() const { return owningManager.get().AssetHasErrorMessages(id); }
const std::vector<std::any>& Asset::ErrorMessages() const { return owningManager.get().GetErrorMessagesOfAsset(id); }

SubAssetList::Iterator::Iterator(Tree<UUID>::ChildrenList::NodeIterator itr, const Assets& owningManager)  noexcept :
    itr(itr),
    owningManager(owningManager) {}

SubAssetList::Iterator::reference SubAssetList::Iterator::operator*() const  noexcept { return owningManager.FindAsset(*itr).Value(); }
SubAssetList::Iterator::pointer SubAssetList::Iterator::operator->() const noexcept { return owningManager.FindAsset(*itr); }

SubAssetList::Iterator& SubAssetList::Iterator::operator++()  noexcept { ++itr; return *this; }
SubAssetList::Iterator SubAssetList::Iterator::operator++(int) noexcept { Iterator tmp = *this; ++(*this); return tmp; }

SubAssetList::SubAssetList(Tree<UUID>::ChildrenList childrenList, const Assets& owningManager) noexcept :
    childrenList(childrenList),
    owningManager(owningManager) {};

Asset& SubAssetList::operator[](std::size_t idx)  noexcept { return owningManager.FindAsset(childrenList[idx]).Value(); }
const Asset& SubAssetList::operator[](std::size_t idx) const noexcept { return owningManager.FindAsset(childrenList[idx]).Value(); }

SubAssetList::Iterator SubAssetList::begin() noexcept { return Iterator(childrenList.begin(), owningManager); }
SubAssetList::Iterator SubAssetList::end() noexcept { return Iterator(childrenList.end(), owningManager); }

size_t SubAssetList::size() const  noexcept { return childrenList.size(); }