#pragma once

#include <Engine/Graphics.hpp>

#include <array>
#include <string>
#include <variant>

struct GPUBuffer;
struct GPUTexture;
struct GPUSampler;

// Descriptor Set
enum class DescriptorType {
    UniformBuffer,
    StorageBuffer,
    CombinedImageSampler,
    // TODO implement these in the future
    // Sampler,
    // SampledImage,
    // StorageImage,
    // UniformTexelBuffer,
    // StorageTexelBuffer,
    // InputAttachment
};
struct DescriptorBinding {
    struct UniformBuffer {
        std::reference_wrapper<const GPUBuffer> Buffer;
    };
    struct StorageBuffer {
        std::reference_wrapper<const GPUBuffer> Buffer;
    };
    struct CombinedImageSampler {
        std::reference_wrapper<const GPUTexture> Texture;
        std::reference_wrapper<const GPUSampler> Sampler;
    };

    unsigned BindingSlot{};
    std::variant<std::monostate, UniformBuffer, StorageBuffer, CombinedImageSampler> Descriptor{};
};

using DescriptorSetAllocatorMessage = std::string;
constexpr unsigned MaxDescriptorBinding = 16;

struct GPUDescriptorSet {
    std::array<DescriptorBinding, MaxDescriptorBinding> Bindings{};
};
struct GPUDescriptorSetBuilder {

    GPUDescriptorSetBuilder& SetUniformBufferBinding(unsigned binding, const GPUBuffer& buffer) noexcept;
    GPUDescriptorSetBuilder& SetStorageBufferBinding(unsigned binding, const GPUBuffer& buffer) noexcept;
    GPUDescriptorSetBuilder& SetCombinedImageSamplerBinding(unsigned binding, const GPUTexture& texture, const GPUSampler& sampler) noexcept;

    [[nodiscard]] std::pair<std::optional<GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>> Build() noexcept;

private:
    int idx{};
    std::array<DescriptorBinding, MaxDescriptorBinding> bindings{};
};