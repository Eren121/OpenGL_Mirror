#include "Scene.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>

Scene::Scene(std::filesystem::path assets)
    : model{assets / "cube.obj"}
{
}

void Scene::draw(gl::Shader& shader, Uniforms base) const
{
    // Model on the floor
    {
        Uniforms uniforms = base;
        const float angle = static_cast<float>(glfwGetTime());
        uniforms.texture = 1;
        uniforms.model = glm::rotate(uniforms.model, angle, {1, 1, 0});
        uniforms.send(shader);
        model.draw(shader);
    }
    
    // Floor
    {
        Uniforms uniforms = base;
        uniforms.texture = 1;
        uniforms.model = glm::translate(uniforms.model, {0, -3, 0});
        uniforms.model = glm::scale(uniforms.model, {10.0f, 0.1f, 10.0f});
        uniforms.send(shader);
        model.draw(shader);
    }
}

void Scene::resetGL() const
{
    glDisable(GL_CULL_FACE);
    
    glActiveTexture(GL_TEXTURE0);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void Scene::clear() const
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Scene::drawMirror(gl::Shader& shader, Uniforms uniforms) const
{
    // Where the mirror is drawn, the stencil buffer will contain 1
    // (If the current framebuffer has not stencil buffer, it will just not be written)
    
    glStencilFunc(GL_ALWAYS, 1, 0xff); // To set the reference to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Where the triangle is drawn, set 1 in stencil buffer
    
    uniforms.send(shader);
    
    gl::raii::VertexArray vao;
    gl::raii::Buffer vbo;
    
    obj::Vertex vertices[4];
    vertices[0].pos = {-.5, -.5, 0};
    vertices[1].pos = {.5, -.5, 0};
    vertices[2].pos = {.5, .5, 0};
    vertices[3].pos = {-.5, .5, 0};
    
    vertices[0].texCoords = {0, 0};
    vertices[1].texCoords = {1, 0};
    vertices[2].texCoords = {1, 1};
    vertices[3].texCoords = {0, 1};
    
    vertices[0].nor = {0, 0, 1};
    vertices[1].nor = {0, 0, 1};
    vertices[2].nor = {0, 0, 1};
    vertices[3].nor = {0, 0, 1};
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(obj::AttrVertex);
    gl::vertexAttribPointer(obj::AttrVertex, 3, GL_FLOAT, &obj::Vertex::pos);
    
    glEnableVertexAttribArray(obj::AttrNormal);
    gl::vertexAttribPointer(obj::AttrNormal, 3, GL_FLOAT, &obj::Vertex::nor);
    
    glEnableVertexAttribArray(obj::AttrTextCoords);
    gl::vertexAttribPointer(obj::AttrTextCoords, 2, GL_FLOAT, &obj::Vertex::texCoords);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    
    glStencilFunc(GL_ALWAYS, 0, 0xff); // To set the reference to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Where the triangle is drawn, set 1 in stencil buffer
}

Uniforms::Uniforms()
    : time{static_cast<float>(glfwGetTime())}
{
}

void Uniforms::send(gl::Shader& shader) const
{
    // Generic
    shader.setUniform("u_Time", time);
    
    // Matrices
    shader.setUniform("u_ProjectionMatrix", proj);
    shader.setUniform("u_ViewMatrix", view);
    shader.setUniform("u_ModelMatrix", model);
    
    // Material
    shader.setUniform("u_Texture", texture);
    shader.setUniform("u_AmbientIntensity", ambient);
    shader.setUniform("u_DiffuseIntensity", diffuse);
    shader.setUniform("u_DiffuseColor", diffuseColor);
    shader.setUniform("u_SpecularExponent", specularExponent);
    shader.setUniform("u_SpecularColor", glm::vec4{1, 1, 0, 1});
    shader.setUniform("u_Opacity", opacity);
    
    // Lighting
    shader.setUniform("u_LightDirection", lightDir);
}