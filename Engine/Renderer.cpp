//#include "Renderer.hpp"
//
//#include <GL/glew.h>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "Model.hpp"
//
//#include "OrthoCamera.hpp"
//#include "PerspectiveCamera.hpp"
//
//#include "Texture.hpp"
//#include "Core.hpp"
//#include "TransformComponent.hpp"
//
//#include "Log.hpp"
//
//static constexpr int defaultSize = 10;
//static constexpr int defaultGrowthFactor = 2;
//
////-----------------------------------------------------------------
//
//void Renderer::Register(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model) {
//    auto ptr = shader.lock();
//    ptr->Use();
//    auto it = _current.find(ptr);
//    if (it != _current.end()) {
//        auto modelPtr = model.lock();
//        for (auto& tuple : it->second) {
//            auto& [model, instances, size, vbo] = tuple;
//            if (modelPtr == model.lock()) {
//                return;
//            }
//        }
//    }
//
//    it = _previous.find(ptr);
//    std::vector<Entity> vec;
//    if (it != _previous.end()) {
//        for (auto& tuple : it->second) {
//            auto& [prevModel, prevInstance, prevSize, prevVbo] = tuple;
//            if (prevModel.lock() == model.lock()) {
//                vec.reserve(prevSize);
//                _current[ptr].emplace_back(std::make_tuple(model, vec, prevSize, prevVbo));
//                prevVbo = 0; // previous will get wiped, null prevVbo so it stays alive! :)
//                return;
//            }
//        }
//    }
//
//    // if we are here, didn't return in previous if clause, therefore we need a new vbo for our mesh
//    auto v4s = sizeof(glm::vec4);
//    auto m4s = sizeof(glm::mat4);
//
//    VBO vbo;
//    glCreateBuffers(1, &vbo);
//    glNamedBufferData(vbo, defaultSize * m4s, nullptr, GL_DYNAMIC_DRAW);
//
//    /*
//    for (auto& mesh : model.lock()->_meshes) {
//        glVertexArrayVertexBuffer(mesh.VertexArrayObject(), 1, vbo, 0, static_cast<GLsizei>(m4s));
//
//        glEnableVertexArrayAttrib(mesh.VertexArrayObject(), 0);
//        glEnableVertexArrayAttrib(mesh.VertexArrayObject(), 1);
//        glEnableVertexArrayAttrib(mesh.VertexArrayObject(), 2);
//        glEnableVertexArrayAttrib(mesh.VertexArrayObject(), 3);
//
//        glVertexArrayAttribBinding(mesh.VertexArrayObject(), 0, 1);
//        glVertexArrayAttribBinding(mesh.VertexArrayObject(), 1, 1);
//        glVertexArrayAttribBinding(mesh.VertexArrayObject(), 2, 1);
//        glVertexArrayAttribBinding(mesh.VertexArrayObject(), 3, 1);
//
//        glVertexArrayAttribFormat(mesh.VertexArrayObject(), 0, 4, GL_FLOAT, GL_FALSE, static_cast<GLuint>(v4s * 0));
//        glVertexArrayAttribFormat(mesh.VertexArrayObject(), 1, 4, GL_FLOAT, GL_FALSE, static_cast<GLuint>(v4s * 1));
//        glVertexArrayAttribFormat(mesh.VertexArrayObject(), 2, 4, GL_FLOAT, GL_FALSE, static_cast<GLuint>(v4s * 2));
//        glVertexArrayAttribFormat(mesh.VertexArrayObject(), 3, 4, GL_FLOAT, GL_FALSE, static_cast<GLuint>(v4s * 3));
//
//        glVertexArrayBindingDivisor(mesh.VertexArrayObject(), 1, 1);
//    }
//    */
//    vec.reserve(defaultSize);
//    _current[ptr].emplace_back(std::make_tuple(model, vec, defaultSize, vbo));
//}
//
//void Renderer::Submit(std::weak_ptr<Shader> shader, std::weak_ptr<Model> model, Entity instance) {
//    auto modelPtr = model.lock();
//    auto& vector = _current.at(shader.lock());
//    for (auto& tuple : vector) {
//        auto& [model, instances, size, vbo] = tuple;
//        if (modelPtr == model.lock()) {
//            instances.push_back(instance);
//            return;
//        }
//    }
//}
//
//void Renderer::Render(entt::registry& registry, ACamera* cam) {
//    stats.vertices = 0;
//    stats.indices = 0;
//    stats.drawCalls = 0;
//
//    std::vector<VBO> toBeDeleted;
//    for (auto& pair : _previous) {
//        for (auto& element : pair.second) {
//            toBeDeleted.push_back(std::get<VBO>(element));
//        }
//    }
//    glDeleteBuffers(static_cast<GLsizei>(toBeDeleted.size()), toBeDeleted.data());
//    // done
//    for (auto& pair : _current) {
//        auto& shader = pair.first;
//        shader->Use();
//        shader->UniformMat4("viewProjMatrix", glm::value_ptr(cam->_viewProjectionMatrix));
//        for (auto& tuple : pair.second) {
//            auto& [model, instances, size, vbo] = tuple;
//            auto modelPtr = model.lock();
//
//            auto v4s = sizeof(glm::vec4);
//            auto m4s = sizeof(glm::mat4);
//            if (instances.size() > size) {
//                // need to realloc! find new size
//                while (size <= instances.size()) {
//                    size *= defaultGrowthFactor;
//                }
//                // realloc!
//                glNamedBufferData(vbo, size * m4s, nullptr, GL_DYNAMIC_DRAW);
//            }
//
//            std::vector<glm::mat4> transforms;
//            transforms.reserve(size);
//            for (auto entity : instances) {
//                //transforms.emplace_back(CreateModelMatrixFromTransform(registry.get<ScriptComponent>(entity)["Transform"]));
//            }
//            glNamedBufferSubData(vbo, 0, transforms.size() * m4s, transforms.data());
//
//
//            //for (auto& mesh : modelPtr->_meshes) {
//            //    //FindTexture("!!missing!!").lock()->Bind(); // override possible overrides to missing texture.
//            //    int i = 1;
//            //    /*
//            //    for (auto& tex : mesh._textures) {
//            //        if (i == 17) {
//            //            DOA_LOG_WARNING("Model %s has too many textures! Rendering may look incorrect.", modelPtr->_name);
//            //            break; //too many textures...
//            //        }
//            //        tex.lock()->Bind(i++);
//            //    }
//            //    */
//            //    glBindVertexArray(mesh.VertexArrayObject());
//            //    if (mesh.HasIndexBuffer()) {
//            //        glDrawElementsInstanced(GL_TRIANGLES, mesh.Indices().size(), GL_UNSIGNED_INT, 0, instances.size());
//            //    } else {
//            //        glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.Vertices().size(), instances.size());
//            //    }
//            //    glBindVertexArray(0);
//
//            //    stats.vertices += mesh.Vertices().size();
//            //    stats.indices += mesh.Indices().size();
//            //    stats.drawCalls++;
//            //}
//        }
//    }
//
//    _previous = std::move(_current);
//    _current.clear(); // _current is in an undefined state, clear() brings it back to a defined one
//}
//
//void Renderer::Submit(const std::weak_ptr<Mesh> mesh, const std::weak_ptr<Shader> shader, const glm::mat4& transform, const glm::mat4& viewProjMatrix, const ShaderUniformTable& uniforms) {
//    //drawCalls[shader].emplace_back(mesh, transform, viewProjMatrix, uniforms);
//}
//
//RendererSystem::RendererSystem(Renderer& renderer) noexcept :
//    renderer(renderer) {}
//
//void RendererSystem::Init(Registry& reg) {}
//
//void RendererSystem::Execute(Registry& reg, float deltaTime) {
//    /*
//    Renderer& r = renderer.get();
//
//    for (auto& [shader, drawDatas] : r.drawCalls) {
//        auto s = shader.lock();
//        s->Use();
//
//        for(auto& drawData : drawDatas) {
//            s->UniformMat4("model", &(drawData.model[0][0]));
//            s->UniformMat4("viewProj", &(drawData.viewProj[0][0]));
//            //s->Apply(drawData.uniforms);
//
//            auto mesh = drawData.meshData.lock();
//            glBindVertexArray(mesh->VertexArrayObject());
//
//            glDrawElements(GL_TRIANGLES, 0, GL_UNSIGNED_SHORT, mesh->Indices().data());
//        }
//    }
//
//
//    r.drawCalls.clear();
//    */
//}
