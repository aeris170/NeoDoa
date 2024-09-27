#include <Engine/Sampler.hpp>

#include <Engine/SamplerSerializer.hpp>
#include <Engine/SamplerDeserializer.hpp>

std::string Sampler::Serialize() const noexcept { return SerializeSampler(*this); }
Sampler Sampler::Deserialize(const std::string_view data) noexcept { return DeserializeSampler(data).deserializedSampler; }

Sampler Sampler::Copy(const Sampler& sampler) noexcept { return sampler; }