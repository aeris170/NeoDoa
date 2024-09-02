#pragma once

#include <tuple>
#include <memory>
#include <unordered_map>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Entity.hpp"
#include "Registry.hpp"

typedef GLuint VBO;

struct Model;
struct Mesh;
struct Shader;
struct ACamera;

struct Renderer {

    struct DrawCallData {
        const std::weak_ptr<Mesh> meshData;
        const glm::mat4 model;
        const glm::mat4 viewProj;
        //const ShaderUniformTable uniforms;
    };
    //entt::dense_hash_map<std::weak_ptr<Shader>, std::vector<DrawCallData>> drawCalls;

    std::unordered_map<std::shared_ptr<Shader>, std::vector<std::tuple<std::weak_ptr<Model>, std::vector<Entity>, int, VBO>>> _previous;
    std::unordered_map<std::shared_ptr<Shader>, std::vector<std::tuple<std::weak_ptr<Model>, std::vector<Entity>, int, VBO>>> _current;

    struct Stats {
        int vertices{ 0 };
        int indices{ 0 };
        int drawCalls{ 0 };
    } stats;

    void Register(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model);
    void Submit(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model, Entity instance);

    void Render(entt::registry& registry, ACamera* cam);


    //void Submit(const std::weak_ptr<Mesh> mesh, const std::weak_ptr<Shader> shader, const glm::mat4& transform, const glm::mat4& viewProjMatrix, const ShaderUniformTable& uniforms);
};

struct RendererSystem {
    std::reference_wrapper<Renderer> renderer;

    RendererSystem(Renderer& renderer) noexcept;

    void Init(Registry& reg);
    void Execute(Registry& reg, float deltaTime);
};