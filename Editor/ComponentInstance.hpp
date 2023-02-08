#pragma once

#include <any>
#include <string>
#include <vector>
#include <string_view>

#include <UUID.hpp>
#include <Assets.hpp>
#include <Component.hpp>

#include <ObserverPattern.hpp>

enum class InstantiationError {
    OK = 0,
    DEFINITION_MISSING,
    NON_DEFITION_INSTANTIATION,
    DEFINITION_COMPILE_ERROR,
    DEFINITION_NOT_DESERIALIZED,
    _COUNT
};

struct ComponentInstance : public ObserverPattern::Observer {

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

    explicit ComponentInstance(AssetHandle componentAsset) noexcept;
    ComponentInstance(AssetHandle componentAsset, std::vector<Field>&& data) noexcept;
    explicit ComponentInstance(UUID supposedAssetID, InstantiationError error) noexcept;
    ~ComponentInstance() noexcept override;
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

protected:
    void OnNotify(const ObserverPattern::Observable* source, ObserverPattern::Notification message) override;

private:
    AssetHandle componentAsset{};
    std::vector<Field> memberValues{};
    InstantiationError error{ InstantiationError::OK };
    UUID supposedAssetID{ UUID::Empty() }; /* only applicable when error != OK */

    static void FillData(const std::vector<Component::Field>& fields, std::vector<Field>& data);
    static void ReConstructData(const std::vector<Component::Field>& fields, std::vector<Field>& data);
    static void CreateNewEntry(const std::string& type, const std::string& name, std::vector<Field>& data);
};