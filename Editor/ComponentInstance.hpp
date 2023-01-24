#pragma once

#include <any>
#include <vector>

#include <Assets.hpp>
#include <Component.hpp>

#include <ObserverPattern.hpp>

struct ComponentInstance : public ObserverPattern::Observer {

    struct Field {

        Field(const std::string& typeName, const std::string& name, const std::any& value) noexcept;

        std::string TypeName();
        const std::string& TypeName() const;

        std::string Name();
        const std::string& Name() const;

        template<typename T>
        T& As() { return std::any_cast<T&>(value); }

    private:
        std::string typeName;
        std::string name;
        std::any value;
    };

    explicit ComponentInstance(AssetHandle componentAsset) noexcept;
    ~ComponentInstance() noexcept override;
    ComponentInstance(const ComponentInstance& other) = delete;
    ComponentInstance(ComponentInstance&& other) noexcept = default;
    ComponentInstance& operator=(const ComponentInstance& other) = delete;
    ComponentInstance& operator=(ComponentInstance&& other) noexcept = default;

    UUID ComponentAssetID() const;
    std::vector<Field>& MemberValues();
    const std::vector<Field>& MemberValues() const;

protected:
    void OnNotify(const ObserverPattern::Observable* source, ObserverPattern::Notification message) override;

private:
    AssetHandle componentAsset{};
    std::vector<Field> memberValues{};

    static void FillData(const std::vector<Component::Field>& fields, std::vector<Field>& data);
    static void ReConstructData(const std::vector<Component::Field>& fields, std::vector<Field>& data);
    static void CreateNewEntry(const std::string& type, const std::string& name, std::vector<Field>& data);
};