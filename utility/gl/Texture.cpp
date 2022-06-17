#include "Texture.hpp"
#include <iostream>

// Defines as static to avoid clashes in cases of others files also include stb_image
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gl
{
    void Texture::load1x1White()
    {
        static const GLubyte pixels[] = {
                0xff, 0xff, 0xff, 0xff // RGBA Opaque white 1x1
        };
        
        Texture::bind(this);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    void Texture::load(const std::filesystem::path& path)
    {
        // Set to the way OpenGL expect pixels
        stbi_set_flip_vertically_on_load(true);
        
        int width, height, channels;
        unsigned char *pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
        
        if(!pixels)
        {
            std::cerr << "Failed to load the texture from the path " << path << std::endl;
        }
        else
        {
            Texture::bind(this);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        
        free(pixels);
        pixels = nullptr;
    }
    
    void Texture::load(glm::ivec2 size, glm::vec4 color)
    {
        std::vector<glm::vec4> pixels(size.x * size.y, color);
        
        Texture::bind(this);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_FLOAT, pixels.data());
    }
    
    void Texture::load(glm::ivec2 size, GLint internalFormat)
    {
        Texture::bind(this);
        
        // Since we pass nullptr as data, we can set type to any valid type because there is no data at all.
        // format should be the same as internalFormat, since there is no data but sometimes it will not work,
        // For example using internalFormat=GL_DEPTH_COMPONENT and format=GL_RGBA will not work.
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, internalFormat, GL_UNSIGNED_BYTE, nullptr);
    }
    
    void Texture::bind(const Texture *texture)
    {
        if (texture)
        {
            glBindTexture(GL_TEXTURE_2D, texture->m_texture);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    unsigned int Texture::getID() const
    {
        return m_texture.id;
    }
    
    void Texture::setFilter(Texture::Filter filter)
    {
        int gl_filter;
        
        switch (filter)
        {
            case Linear:
                gl_filter = GL_LINEAR;
                break;
            
            case Nearest:
                gl_filter = GL_NEAREST;
                break;
        }
        
        Texture::bind(this);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    }
    
    glm::vec2 Texture::getSize() const
    {
        int miplevel = 0;
        int w, h;
        
        Texture::bind(this);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);
        
        return {w, h};
    }
}