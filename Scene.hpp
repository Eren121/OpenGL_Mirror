#pragma once

#include "Model.hpp"
#include <utility/gl/Shader.hpp>

// add a bit utilities functions...
namespace glm
{
    namespace
    {
        /// @brief Compute change-of-basis matrix.
        /// From basis {e1, e2, e3} to canonical basis of R^3.
        mat4 transition(const glm::vec3& e1, const glm::vec3& e2, const glm::vec3& e3)
        {
            // GLM is column major: mat4(col1, col2, col3, col4)
            return {
                    vec4(e1, 0),
                    vec4(e2, 0),
                    vec4(e3, 0),
                    vec4(vec3(0), 1)
            };
        }
    }
}

struct Uniforms
{
    Uniforms();
    
    void send(gl::Shader& shader) const;
    
    glm::mat4 proj{1};
    glm::mat4 view{1};
    glm::mat4 model{1};
    float time{0};
    float ambient{0};
    float diffuse{1};
    float specularExponent{16.0f};
    unsigned int texture{0};
    glm::vec3 lightDir;
  
    float opacity{1};
    
    glm::vec4 diffuseColor{1};
};

class Scene
{
public:
    Scene(std::filesystem::path assets);
    
    void resetGL() const;
    void clear() const;
    
    void draw(gl::Shader& shader, Uniforms uniforms = {}) const;
    void drawMirror(gl::Shader& shader, Uniforms uniforms) const;
    
    obj::Model model;
};

