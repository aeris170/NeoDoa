#pragma once

#include <vector>
#include <iomanip>
#include <unordered_map>

#include <entt/entt.hpp>

#include <Engine/Log.hpp>
#include <Engine/UUID.hpp>
#include <Engine/Assets.hpp>
#include <Engine/GPUShader.hpp>
#include <Engine/GPUTexture.hpp>
#include <Engine/GPUFrameBuffer.hpp>

struct AssetGPUBridge;

template<typename T, typename ErrorMessageType>
struct GPUObjectDatabase {
#if DEBUG
    using Database = std::unordered_map<UUID, T>;
#elif NDEBUG
    using Database = entt::dense_map<UUID, T>;
#else
#error "Neither DEBUG nor NDEBUG are defined!"
#endif

    explicit GPUObjectDatabase(AssetGPUBridge& bridge) noexcept :
        bridge(bridge) {}
    ~GPUObjectDatabase() noexcept = default;
    GPUObjectDatabase(GPUObjectDatabase&) noexcept = delete;
    GPUObjectDatabase(GPUObjectDatabase&&) noexcept = delete;
    GPUObjectDatabase& operator=(GPUObjectDatabase&) noexcept = delete;
    GPUObjectDatabase& operator=(GPUObjectDatabase&&) noexcept = delete;

    bool Exists(const UUID asset) const noexcept { return database.contains(asset); }
    T& Fetch(const UUID asset) noexcept {
        assert(Exists(asset));
        return database[asset];
    }
    const T& Fetch(const UUID asset) const noexcept {
        assert(Exists(asset));
        return database.at(asset);
    }
    T* Query(const UUID asset) noexcept {
        if (!Exists(asset)) { return nullptr; }
        return &Fetch(asset);
    }
    const T* Query(const UUID asset) const noexcept {
        if (!Exists(asset)) { return nullptr; }
        return &Fetch(asset);
    }

    std::vector<ErrorMessageType> Allocate(const Assets& assets, const UUID asset) noexcept { DOA_LOG_FATAL("Illegal allocator! %s", std::quoted(assets.FindAsset(asset)->File().Name())); std::unreachable(); };
    std::vector<ErrorMessageType> TryAllocate(const Assets& assets, const UUID asset) noexcept {
        if (Exists(asset)) { return; }
        Allocate(assets, asset);
    }
    void Deallocate(const UUID asset) noexcept {
        database.erase(asset);
    }

    const T& Missing() const noexcept {
        DOA_LOG_FATAL("Illegal missing resource!"); std::unreachable();
    }

private:
    AssetGPUBridge& bridge;
    Database database{};
};

#define ND_EXPLICIT_SPECIALIZE_ALLOCATOR(Name, T, ErrorMessageType) \
using Name = GPUObjectDatabase<T, ErrorMessageType>; \
template<> \
std::vector<ErrorMessageType> Name::Allocate(const Assets& assets, const UUID asset) noexcept
#define ND_EXPLICIT_SPECIALIZE_ALLOCATOR_SPECIALIZE_MISSING(Name, T) \
template<> \
const T& Name::Missing() const noexcept
ND_EXPLICIT_SPECIALIZE_ALLOCATOR(GPUFrameBuffers, GPUFrameBuffer, FrameBufferAllocatorMessage);
ND_EXPLICIT_SPECIALIZE_ALLOCATOR(GPUShaders, GPUShader, ShaderCompilerMessage);
ND_EXPLICIT_SPECIALIZE_ALLOCATOR(GPUShaderPrograms, GPUShaderProgram, ShaderLinkerMessage);
ND_EXPLICIT_SPECIALIZE_ALLOCATOR(GPUSamplers, GPUSampler, SamplerAllocatorMessage);
ND_EXPLICIT_SPECIALIZE_ALLOCATOR(GPUTextures, GPUTexture, TextureAllocatorMessage); ND_EXPLICIT_SPECIALIZE_ALLOCATOR_SPECIALIZE_MISSING(GPUTextures, GPUTexture);
#undef ND_EXPLICIT_SPECIALIZE_ALLOCATOR
#undef ND_EXPLICIT_SPECIALIZE_ALLOCATOR_SPECIALIZE_MISSING

struct AssetGPUBridge {

    GPUFrameBuffers& GetGPUFrameBuffers() noexcept;
    const GPUFrameBuffers& GetGPUFrameBuffers() const noexcept;
    GPUShaders& GetShaders() noexcept;
    const GPUShaders& GetShaders() const noexcept;
    GPUShaderPrograms& GetShaderPrograms() noexcept;
    const GPUShaderPrograms& GetShaderPrograms() const noexcept;
    GPUSamplers& GetSamplers() noexcept;
    const GPUSamplers& GetSamplers() const noexcept;
    GPUTextures& GetTextures() noexcept;
    const GPUTextures& GetTextures() const noexcept;

private:
    GPUFrameBuffers gpuFrameBuffers{ *this };
    GPUShaders gpuShaders{ *this };
    GPUShaderPrograms gpuShaderPrograms{ *this };
    GPUSamplers gpuSamplers{ *this };
    GPUTextures gpuTextures{ *this };

public:
    AssetGPUBridge() noexcept = default;
    ~AssetGPUBridge() noexcept = default;
    AssetGPUBridge(AssetGPUBridge&) noexcept = delete;
    AssetGPUBridge(AssetGPUBridge&&) noexcept = default;
    AssetGPUBridge& operator=(AssetGPUBridge&) noexcept = delete;
    AssetGPUBridge& operator=(AssetGPUBridge&&) noexcept = default;
};
