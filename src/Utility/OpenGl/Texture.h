#pragma once

#include <array>
#include <vector>
#include <string>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

namespace Rutile {
    enum class TextureFormat {
        R = GL_RED,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        DEPTH_COMPONENT = GL_DEPTH_COMPONENT
    };

    enum class TextureStorageType {
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
        FLOAT = GL_FLOAT
    };

    enum class TextureWrapMode {
        REPEAT = GL_REPEAT,
        CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE
    };

    enum class TextureFilteringMode {
        LINEAR = GL_LINEAR,
        NEAREST = GL_NEAREST
    };

    struct TextureParameters {
        TextureParameters(
            TextureFormat imageFormat = TextureFormat::RGBA,
            TextureStorageType internalStorageType = TextureStorageType::UNSIGNED_BYTE,
            TextureWrapMode wrapMode = TextureWrapMode::REPEAT,
            TextureFilteringMode filteringMode = TextureFilteringMode::LINEAR
        );

        TextureFormat imageFormat{ };
        TextureStorageType internalStorageType{ };

        TextureWrapMode horizontalWrapMode{ };
        TextureWrapMode verticalWrapMode{ };
        TextureWrapMode depthWrapMode{ };

        TextureFilteringMode minFilter{ };
        TextureFilteringMode magFilter{ };
    };

    class Texture {
    public:
        Texture(unsigned int textureType);
        Texture(const Texture& other) = delete;
        Texture(Texture&& other) noexcept = default;
        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) noexcept = default;
        ~Texture();

        void Bind();
        void Unbind();

        unsigned int Get();

        unsigned int GetType();

    protected:
        unsigned int m_TextureType;

    private:
        unsigned int m_Handle;
    };

    class Texture2D : public Texture {
    public:
        Texture2D(glm::ivec2 size, TextureParameters parameters = TextureParameters{ }, std::vector<unsigned char> data = std::vector<unsigned char>{ });

        Texture2D(const std::string& path, TextureParameters parameters = TextureParameters{ }, bool flip = true);
    };

    class Texture2D_Array : public Texture {
    public:
        Texture2D_Array(const std::vector<std::string>& paths, TextureParameters parameters = TextureParameters{ }, bool flip = true);
    };

    class Cubemap : public Texture {
    public:
        Cubemap(
            glm::ivec2 size, 
            TextureParameters parameters = TextureParameters{ }, 
            std::array<std::vector<unsigned char>, 6> data = std::array<std::vector<unsigned char>, 6>{ 
                std::vector<unsigned char>{ }, 
                std::vector<unsigned char>{ }, 
                std::vector<unsigned char>{ }, 
                std::vector<unsigned char>{ }, 
                std::vector<unsigned char>{ }, 
                std::vector<unsigned char>{ } 
            }
        );
    };
}