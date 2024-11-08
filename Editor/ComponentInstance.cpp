#include <Editor/ComponentInstance.hpp>

ComponentInstance::Field::Field(const std::string& typeName, const std::string& name, const std::any& value) noexcept :
    typeName(typeName),
    name(name),
    value(value) {}

std::string ComponentInstance::Field::TypeName() { return typeName; }
const std::string& ComponentInstance::Field::TypeName() const { return typeName; }

std::string ComponentInstance::Field::Name() { return name; }
const std::string& ComponentInstance::Field::Name() const { return name; }

void ComponentInstance::Field::Reset() {
         if (typeName == "bool")          { value.emplace<bool>    (static_cast<bool>    (0)); }
    else if (typeName == "int8")          { value.emplace<int8_t>  (static_cast<int8_t>  (0)); }
    else if (typeName == "int16")         { value.emplace<int16_t> (static_cast<int16_t> (0)); }
    else if (typeName == "int")           { value.emplace<int32_t> (static_cast<int32_t> (0)); }
    else if (typeName == "long")          { value.emplace<int64_t> (static_cast<int64_t> (0)); }
    else if (typeName == "uint8")         { value.emplace<uint8_t> (static_cast<uint8_t> (0)); }
    else if (typeName == "uint16")        { value.emplace<uint16_t>(static_cast<uint16_t>(0)); }
    else if (typeName == "unsigned int")  { value.emplace<uint32_t>(static_cast<uint32_t>(0)); }
    else if (typeName == "unsigned long") { value.emplace<uint64_t>(static_cast<uint64_t>(0)); }
    else if (typeName == "float")         { value.emplace<float_t> (static_cast<float_t> (0)); }
    else if (typeName == "double")        { value.emplace<double_t>(static_cast<double_t>(0)); }
}

ComponentInstance::ComponentInstance(AssetHandle componentAsset) noexcept :
    componentAsset(componentAsset) {
    componentAsset->AddObserver(*this);
    if (componentAsset->HasDeserializedData()) {
        FillData(componentAsset->DataAs<Component>().fields, memberValues);
    } else {
        error = InstantiationError::DEFINITION_NOT_DESERIALIZED;
    }
}
ComponentInstance::ComponentInstance(AssetHandle componentAsset, std::vector<Field>&& data) noexcept :
    componentAsset(componentAsset) {
    componentAsset->AddObserver(*this);
    memberValues = std::move(data);
    if (componentAsset->HasDeserializedData()) {
        ReConstructData(componentAsset->DataAs<Component>().fields, memberValues);
    } else {
        error = InstantiationError::DEFINITION_NOT_DESERIALIZED;
    }
}
ComponentInstance::ComponentInstance(UUID supposedAssetID, InstantiationError error) noexcept :
    componentAsset(nullptr),
    error(error),
    supposedAssetID(supposedAssetID) {}
ComponentInstance::~ComponentInstance() noexcept {
    if (componentAsset.HasValue()) {
        componentAsset->RemoveObserver(*this);
    }
}

UUID ComponentInstance::ComponentAssetID() const { return componentAsset ? componentAsset->ID() : supposedAssetID; }
std::vector<ComponentInstance::Field>& ComponentInstance::MemberValues() { return memberValues; }
const std::vector<ComponentInstance::Field>& ComponentInstance::MemberValues() const { return memberValues; }

bool ComponentInstance::HasError() const { return error != InstantiationError::OK; }
InstantiationError ComponentInstance::GetError() const { return error; }
std::string_view ComponentInstance::ErrorString() const {
    switch(error) {
        using enum InstantiationError;
    case DEFINITION_MISSING:
        return "Component Definition is missing!";
    case NON_DEFITION_INSTANTIATION:
        return "Instantied (failed) a non-component!";
    case DEFINITION_COMPILE_ERROR:
        return "Component Definition has compiler errors!";
    case DEFINITION_NOT_DESERIALIZED:
        return "Component Definition is not deserialized!";
    default:
        return "";
    }
}

void ComponentInstance::OnNotify(const ObserverPattern::Observable* source, ObserverPattern::Notification message) {
    if (message == "moved"_hs) {
        /* casting-away const is safe here because Assets are never created const */
        componentAsset = AssetHandle(const_cast<Asset*>(static_cast<const Asset*>(source)));
    } else if (message == "deserialized"_hs) {
        /* component has been deserialized, check for compiler errors, */
        if (componentAsset->HasErrorMessages()) {
            error = InstantiationError::DEFINITION_COMPILE_ERROR;
        } else {
            /* if there aren't any we potentially have new/reorganized fields so we must */
            /* reorganize/reconstruct our instance data to reflect the changes on component */
            ReConstructData(componentAsset->DataAs<Component>().fields, memberValues);
            /* we also must clean-up the error */
            error = InstantiationError::OK;
        }
    } else if (message == "destructed"_hs) {
        componentAsset = nullptr;
        error = InstantiationError::DEFINITION_MISSING;
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