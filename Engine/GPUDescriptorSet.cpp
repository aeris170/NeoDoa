#include <Engine/GPUDescriptorSet.hpp>

#include <format>
#include <cassert>

GPUDescriptorSetBuilder& GPUDescriptorSetBuilder::SetName(std::string_view name) noexcept {
#ifdef DEBUG
    this->name = name;
#endif
    return *this;
}
GPUDescriptorSetBuilder& GPUDescriptorSetBuilder::SetUniformBufferBinding(unsigned binding, const GPUBuffer& buffer) noexcept {
    assert(idx < MaxDescriptorBinding && std::format("index out of range expected [0-{}), received {}", MaxDescriptorBinding, idx).c_str());
    bindings[idx].BindingSlot = binding;
    bindings[idx].Descriptor.emplace<DescriptorBinding::UniformBuffer>(buffer);
    idx++;
    return *this;
}
GPUDescriptorSetBuilder& GPUDescriptorSetBuilder::SetStorageBufferBinding(unsigned binding, const GPUBuffer& buffer) noexcept {
    assert(idx < MaxDescriptorBinding && std::format("index out of range expected [0-{}), received {}", MaxDescriptorBinding, idx).c_str());
    bindings[idx].BindingSlot = binding;
    bindings[idx].Descriptor.emplace<DescriptorBinding::StorageBuffer>(buffer);
    idx++;
    return *this;
}
GPUDescriptorSetBuilder& GPUDescriptorSetBuilder::SetCombinedImageSamplerBinding(unsigned binding, const GPUTexture& texture, const GPUSampler& sampler) noexcept {
    assert(idx < MaxDescriptorBinding && std::format("index out of range expected [0-{}), received {}", MaxDescriptorBinding, idx).c_str());
    bindings[idx].BindingSlot = binding;
    bindings[idx].Descriptor.emplace<DescriptorBinding::CombinedImageSampler>(texture, sampler);
    idx++;
    return *this;
}

std::pair<std::optional<GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>> GPUDescriptorSetBuilder::Build() noexcept {
    return Graphics::Builders::Build(*this);
}