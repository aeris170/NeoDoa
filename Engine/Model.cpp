#include <Engine/Model.hpp>

#include <Engine/ModelSerializer.hpp>
#include <Engine/ModelDeserializer.hpp>

std::string Model::Serialize() const noexcept { return "1";/*SerializeModel(*this);*/ }
Model Model::Deserialize(const std::string_view data) noexcept { return DeserializeModel(data).deserializedModel; }

Model Model::Copy(const Model& model) noexcept { return model; }