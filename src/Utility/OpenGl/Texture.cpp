#include "Texture.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Rutile {
    TextureParameters::TextureParameters(
        TextureFormat imageFormat,
        TextureStorageType internalStorageType,
        TextureWrapMode wrapMode,
        TextureFilteringMode filteringMode
    )
        : imageFormat{ imageFormat }
        , internalStorageType(internalStorageType)
        , horizontalWrapMode(wrapMode)
        , verticalWrapMode(wrapMode)
        , depthWrapMode(wrapMode)
        , minFilter(filteringMode)
        , magFilter(filteringMode) { }

    Texture::Texture(unsigned int textureType)
        : m_TextureType(textureType) {

        glGenTextures(1, &m_Handle);
        Bind();
    }

    Texture::~Texture() {
        glDeleteTextures(1, &m_Handle);
    }

    void Texture::Bind() {
        glBindTexture(m_TextureType, m_Handle);
    }

    void Texture::Unbind() {
        glBindTexture(m_TextureType, 0);
    }

    unsigned int Texture::Get() {
        return m_Handle;
    }

    unsigned int Texture::GetType() {
        return m_TextureType;
    }

    Texture2D::Texture2D(glm::ivec2 size, TextureParameters parameters, std::vector<unsigned char> data)
        : Texture(GL_TEXTURE_2D) {

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)parameters.horizontalWrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)parameters.verticalWrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)parameters.minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)parameters.magFilter);

        if (data.empty()) {
            glTexImage2D(GL_TEXTURE_2D, 0, (int)parameters.imageFormat, size.x, size.y, 0, (int)parameters.imageFormat, (int)parameters.internalStorageType, nullptr);
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, (int)parameters.imageFormat, size.x, size.y, 0, (int)parameters.imageFormat, (int)parameters.internalStorageType, data.data());
        }
    }

    Texture2D::Texture2D(const std::string& path, TextureParameters parameters, bool flip)
        : Texture(GL_TEXTURE_2D) {

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)parameters.horizontalWrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)parameters.verticalWrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)parameters.minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)parameters.magFilter);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(flip);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 3);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, (int)parameters.imageFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture: " << path << std::endl;
        }
        stbi_image_free(data);
    }

    Texture2D_Array::Texture2D_Array(const std::vector<std::string>& paths, TextureParameters parameters, bool flip)
        : Texture(GL_TEXTURE_2D_ARRAY) {
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, (int)parameters.horizontalWrapMode);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, (int)parameters.verticalWrapMode);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, (int)parameters.minFilter);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, (int)parameters.magFilter);

        std::vector<std::vector<unsigned char>> data;
        int width{ -1 };
        int height{ -1 };
        int channels{ -1 };

        data.resize(paths.size());

        stbi_set_flip_vertically_on_load(flip);

        size_t i = 0;
        for (auto& path : paths) {
            int w, h, c;
            unsigned char* d = stbi_load(path.c_str(), &w, &h, &c, 3);

            if ((w != width && width != -1) || (h != height && height != -1)) {
                std::cout << "Not all images: " << std::endl;

                for (auto& path : paths) {
                    std::cout << "  " << path << std::endl;
                }

                std::cout << "are the same size, cannot create texture" << std::endl;

                return;
            }

            width = w;
            height = h;
            channels = c;

            if (d) {
                data[i].resize(width * height * 3);

                std::memcpy((void*)data[i].data(), (void*)d, data[i].size());

                stbi_image_free(d);
            }
            else {
                std::cout << "Failed to load texture: " << path << std::endl;
                return;
            }

            ++i;
        }

        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 3, GL_RGB8, width, height, (int)paths.size());

        for (size_t i = 0; i < data.size(); ++i) {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, (int)i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, (void*)data[i].data());
        }

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    Cubemap::Cubemap(glm::ivec2 size, TextureParameters parameters, std::array<std::vector<unsigned char>, 6> data)
        : Texture(GL_TEXTURE_CUBE_MAP) {

        for (int i = 0; i < 6; ++i) {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, (int)parameters.horizontalWrapMode);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, (int)parameters.verticalWrapMode);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (int)parameters.depthWrapMode);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (int)parameters.magFilter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (int)parameters.minFilter);

            if (data[i].empty()) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (int)parameters.imageFormat, size.x,
                    size.y, 0, (int)parameters.imageFormat, (int)parameters.internalStorageType, nullptr);
            }
            else {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (int)parameters.imageFormat, size.x,
                    size.y, 0, (int)parameters.imageFormat, (int)parameters.internalStorageType, data[i].data());
            }
        }
    }
}