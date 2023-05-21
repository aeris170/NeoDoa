#include "OutlineRenderer.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Model.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

#include "OrthoCamera.hpp"
#include "PerspectiveCamera.hpp"

#include "Texture.hpp"

//-----------------------------------------------------------------

void OutlineRenderer::Render(std::vector<std::tuple<Transform&, ModelRenderer&>>& objects, ACamera* cam, glm::vec3 outlineColor) {
    /*
    vertices = 0;
    indices = 0;
    drawCalls = 0;

    std::weak_ptr<Shader> stdShader = FindShader("Simple Shader");
    std::shared_ptr shader = stdShader.lock();
    shader->Use();

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    for (auto& [t, mr] : objects) {
        shader->UniformMat4("modelMatrix", (float*)glm::value_ptr(CreateModelMatrixFromTransform(t)));
        shader->UniformMat4("viewProjMatrix", glm::value_ptr(cam->_viewProjectionMatrix));
        Model*& model = mr.Model();
        for (auto& mesh : model->_meshes) {
            FindTexture("!!missing!!").value().lock()->Bind();
            int i = 1;
            for (auto& tex : mesh._textures) {
                if (i == 17) {
                    break;
                }
                tex.lock()->Bind(i++);
            }

            if (!mr._isActive) {
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            } else {
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }

            glBindVertexArray(mesh._vao);
            if (mesh._ebo > 0) {
                glDrawElements(GL_TRIANGLES, mesh._indices.size(), GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, mesh._vertices.size());
            }

            vertices += mesh._vertices.size();
            indices += mesh._indices.size();
            drawCalls++;
        }
    }
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00); // disable writing to the stencil buffer
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    stdShader = FindShader("Solid Color Shader");
    shader = stdShader.lock();
    shader->Use();
    for (auto& [t, mr] : objects) {
        glm::vec3 origScale = t.Scale();
        t.Scale() += std::clamp(glm::distance(cam->eye, t.Translation()) / 50, 0.01f, 20.0f);
        shader->UniformMat4("modelMatrix", (float*)glm::value_ptr(CreateModelMatrixFromTransform(t)));
        shader->UniformMat4("viewProjMatrix", glm::value_ptr(cam->_viewProjectionMatrix));
        shader->Uniform4f("color", glm::value_ptr(outlineColor));
        t.Scale() = origScale;
        Model*& model = mr.Model();
        for (auto& mesh : model->_meshes) {
            FindTexture("!!missing!!").value().lock()->Bind();
            int i = 1;
            for (auto& tex : mesh._textures) {
                if (i == 17) {
                    break;
                }
                tex.lock()->Bind(i++);
            }

            glBindVertexArray(mesh._vao);
            if (mesh._ebo > 0) {
                glDrawElements(GL_TRIANGLES, mesh._indices.size(), GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, mesh._vertices.size());
            }

            vertices += mesh._vertices.size();
            indices += mesh._indices.size();
            drawCalls++;
        }
    }
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glDisable(GL_STENCIL_TEST);
    */
}