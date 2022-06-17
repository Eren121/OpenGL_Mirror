#pragma once

#include "gl.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>
#include <string>
#include <filesystem>

namespace gl
{
    /// @brief Represent all necessary for representing OpenGL shaders.
    class Shader
    {
    public:
        
        /// @brief Try to load a shader.
        /// @param vertex,fragment The source code for each shader.
        /// @throws If there was an error.
        void load(const std::string& vertexSrc, const std::string& fragmentSrc);
        
        void load(const std::filesystem::path& vert, const std::filesystem::path& frag);
        
        /// @brief Bind the shader.
        /// @param shader Pass nullptr to unbind.
        static void bind(const Shader *shader);
        
        /// @name
        /// @brief Set a uniform variable
        /// @{
        
        void setUniform(const std::string& name, const glm::mat4& value);
        
        void setUniform(const std::string& name, const glm::vec4& value);
        void setUniform(const std::string& name, const glm::vec3& value);
    
        void setUniform(const std::string& name, int value);
        void setUniform(const std::string& name, unsigned int value);
        void setUniform(const std::string& name, float value);
        
        /// @}
    
    private:
        gl::raii::Program m_program;
    };
}