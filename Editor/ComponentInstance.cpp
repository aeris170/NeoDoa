#include "ComponentInstance.hpp"

ComponentInstance::Field::Field(const std::string& typeName, const std::string& name, const std::any& value) noexcept :
    typeName(typeName),
    name(name),
    value(value) {}

std::string ComponentInstance::Field::TypeName() { return typeName; }
const std::string& ComponentInstance::Field::TypeName() const { return typeName; }

std::string ComponentInstance::Field::Name() { return name; }
const std::string& ComponentInstance::Field::Name() const { return name; }

ComponentInstance::ComponentInstance(AssetHandle componentAsset) noexcept :
    componentAsset(componentAsset) {
    componentAsset->AddObserver(*this);
    FillData(componentAsset->DataAs<Component>().fields, memberValues);
}

ComponentInstance::~ComponentInstance() noexcept {
    componentAsset->RemoveObserver(*this);
}

UUID ComponentInstance::ComponentAssetID() const { return componentAsset->ID(); }
std::vector<ComponentInstance::Field>& ComponentInstance::MemberValues() { return memberValues; }
const std::vector<ComponentInstance::Field>& ComponentInstance::MemberValues() const { return memberValues; }

void ComponentInstance::OnNotify(const ObserverPattern::Observable* source, ObserverPattern::Notification message) {
    if (message == "moved"_hs) {
        /* casting-away const is safe here because Assets are never created const */
        componentAsset = AssetHandle(const_cast<Asset*>(static_cast<const Asset*>(source)));
    } else if (message == "deserialized"_hs) {
        /* component has been deserialized, and potentially have new/reorganized fields */
        /* we must reorganize/reconstruct our instance data to reflect the changes on component */
        ReConstructData(componentAsset->DataAs<Component>().fields, memberValues);
    }
}

void ComponentInstance::FillData(const std::vector<Component::Field>& fields, std::vector<ComponentInstance::Field>& data) {
    data.clear();
    for (const auto& field : fields) {
        const auto& type{ field.typeName };
        const auto& name{ field.name };
        CreateNewEntry(type, name, data);
    }
}

void ComponentInstance::ReConstructData(const std::vector<Component::Field>& fields, std::vector<ComponentInstance::Field>& data) {
    /* Here, we will reconstruct instance fields/member values. Reconstruction rules:
        if field was not present in the old data
            create a zeroed out entry
        else if field has a new type
            create a zeroed out entry
        else
            take old entry as-is
    */
    std::vector<ComponentInstance::Field> newData{};
    for (const auto& field : fields) {
        const auto& type{ field.typeName };
        const auto& name{ field.name };

        auto search = std::ranges::find_if(data, [&name](ComponentInstance::Field& f) {
            return name == f.Name();
        });
        if (search == data.end()) {
            /* we have a new field, create a zeroed out entry */
            CreateNewEntry(type, name, newData);
        } else if (type != search->TypeName()) {
            CreateNewEntry(type, name, newData);
        } else {
            newData.emplace_back(std::move(*search));
        }
    }
    data = std::move(newData);
}

void ComponentInstance::CreateNewEntry(const std::string& type, const std::string& name, std::vector<ComponentInstance::Field>& data) {
         if (type == "bool")          { data.emplace_back(type, name, static_cast<bool>    (0)); }
    else if (type == "int8")          { data.emplace_back(type, name, static_cast<int8_t>  (0)); }
    else if (type == "int16")         { data.emplace_back(type, name, static_cast<int16_t> (0)); }
    else if (type == "int")           { data.emplace_back(type, name, static_cast<int32_t> (0)); }
    else if (type == "long")          { data.emplace_back(type, name, static_cast<int64_t> (0)); }
    else if (type == "uint8")         { data.emplace_back(type, name, static_cast<uint8_t> (0)); }
    else if (type == "uint16")        { data.emplace_back(type, name, static_cast<uint16_t>(0)); }
    else if (type == "unsigned int")  { data.emplace_back(type, name, static_cast<uint32_t>(0)); }
    else if (type == "unsigned long") { data.emplace_back(type, name, static_cast<uint64_t>(0)); }
    else if (type == "float")         { data.emplace_back(type, name, static_cast<float_t> (0)); }
    else if (type == "double")        { data.emplace_back(type, name, static_cast<double_t>(0)); }
}