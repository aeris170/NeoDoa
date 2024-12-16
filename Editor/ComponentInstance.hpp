#pragma once

#include <any>
#include <string>
#include <vector>
#include <functional>
#include <string_view>

#include <Engine/UUID.hpp>
#include <Engine/Assets.hpp>
#include <Engine/Component.hpp>

enum class InstantiationError {
    OK = 0,                      // No error
    DEFINITION_MISSING,          // Instance exists but no reference definition found. Cause: Definition file deleted from disk/not imported into Assets.
    NON_DEFITION_INSTANTIATION,  // Should never happen under normal circumstances. Cause: NeoDoa somehow managed to instantiate something that is not a definition. How? No idea.
    DEFINITION_COMPILE_ERROR,    // Instantiation failed due to compiler errors present in the definition file.
    DEFINITION_NOT_DESERIALIZED, // Definition file exists and is imported, but, contents are not deserialized.
    _COUNT
};

struct ComponentInstance {

    struct Field {

        Field(const std::string& typeName, const std::string& name, const std::any& value) noexcept;

        std::string TypeName();
        const std::string& TypeName() const;

        std::string Name();
        const std::string& Name() const;

        void Reset();

        template<typename T>
        T& As() { return std::any_cast<T&>(value); }

        template<typename T>
        const T& As() const { return std::any_cast<const T&>(value); }

    private:
        std::string typeName;
        std::string name;
        std::any value;
    };

    explicit ComponentInstance(const UUID uuid, Assets& assets) noexcept;
    ComponentInstance(const UUID uuid, Assets& assets, std::vector<Field>&& data) noexcept;
    explicit ComponentInstance(const UUID uuid, Assets& assets, InstantiationError error) noexcept;
    ~ComponentInstance() noexcept;
    ComponentInstance(const ComponentInstance& other) = delete;
    ComponentInstance(ComponentInstance&& other) noexcept = default;
    ComponentInstance& operator=(const ComponentInstance& other) = delete;
    ComponentInstance& operator=(ComponentInstance&& other) noexcept = default;

    UUID ComponentAssetID() const;
    std::vector<Field>& MemberValues();
    const std::vector<Field>& MemberValues() const;

    bool HasError() const;
    InstantiationError GetError() const;
    std::string_view ErrorString() const;

private:
    UUID uuid{ UUID::Empty() };
    std::vector<Field> memberValues{};
    InstantiationError error{ InstantiationError::OK };

    std::reference_wrapper<Assets> referenceScriptsOwningManager;
    decltype(Assets::Events::OnAssetDeserialized)::CallbackHandle onAssetDeserializedHandle;
    decltype(Assets::Events::OnAssetDataDeleted)::CallbackHandle onAssetDataDeletedHandle;
    decltype(Assets::Events::OnAssetDestructed)::CallbackHandle onAssetDestructedHandle;

    void OnAssetDeserialized(const UUID uuid) noexcept;
    void OnAssetDataDeleted(const UUID uuid) noexcept;
    void OnAssetDestructed(const UUID uuid) noexcept;

    static void FillData(const std::vector<Component::Field>& fields, std::vector<Field>& data);
    static void ReConstructData(const std::vector<Component::Field>& fields, std::vector<Field>& data);
    static void CreateNewEntry(const std::string& type, const std::string& name, std::vector<Field>& data);
};