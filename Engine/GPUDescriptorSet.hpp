#pragma once

#include <array>
#include <string>
#include <vector>
#include <utility>
#include <variant>
#include <optional>
#include <string_view>

#include <Engine/Graphics.hpp>

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

struct GPUDescriptorSet {
#ifdef DEBUG
    std::string Name{};
#endif
    std::array<DescriptorBinding, MaxDescriptorBinding> Bindings{};
};
struct GPUDescriptorSetBuilder {
    GPUDescriptorSetBuilder& SetName(std::string_view name) noexcept;
    GPUDescriptorSetBuilder& SetUniformBufferBinding(unsigned binding, const GPUBuffer& buffer) noexcept;
    GPUDescriptorSetBuilder& SetStorageBufferBinding(unsigned binding, const GPUBuffer& buffer) noexcept;
    GPUDescriptorSetBuilder& SetCombinedImageSamplerBinding(unsigned binding, const GPUTexture& texture, const GPUSampler& sampler) noexcept;

    [[nodiscard]] std::pair<std::optional<GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>> Build() noexcept;

private:
#ifdef DEBUG
    std::string name{};
#endif
    int idx{};
    std::array<DescriptorBinding, MaxDescriptorBinding> bindings{};

public:
    ND_GRAPHICS_BUILDER_RULE_OF_0(GPUDescriptorSetBuilder);

private:
#ifdef OPENGL_4_6_SUPPORT
    friend std::pair<std::optional<GPUDescriptorSet>, std::vector<DescriptorSetAllocatorMessage>> Graphics::OpenGL::Build(GPUDescriptorSetBuilder&) noexcept;
#endif
};