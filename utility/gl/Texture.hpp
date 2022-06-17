#pragma once

#include "gl.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <filesystem>

namespace gl
{
    class Texture
    {
    public:
        Texture() = default;
        
        /// @brief Set the texture as 1x1 opaque white
        void load1x1White();
        
        /// @brief Load from an image file on the disk.
        void load(const std::filesystem::path& path);
        
        /// @brief Load as texture of this size with an undefined color
        /// @param internalFormat This function is also the only way to pass a custom argument to the internal
        /// format of the texture. If you use shadows for example, we may use something else than GL_RGBA.
        void load(glm::ivec2 size, GLint internalFormat = GL_RGBA);
        
        /// @brief Load as texture of this size with an uniform color
        void load(glm::ivec2 size, glm::vec4 color);
        
        static void bind(const Texture *texture);
        
        /// @brief Get the size of the texture in pixel.
        glm::vec2 getSize() const;
        
        /// @brief Get the OpenGL ID of the texture.
        unsigned int getID() const;
        
        enum Filter
        {
            Nearest, ///< GL_NEAREST
            Linear ///< GL_LINEAR
        };
        
        /// @brief Set the filtering method. Default's to nearest.
        void setFilter(Filter filter);
    
    private:
        gl::raii::Texture m_texture;
    };
}