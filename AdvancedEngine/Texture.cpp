#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <windows.h>
#include <filesystem>

Texture::Texture(std::vector<const char*> paths) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 0, totalHeight = 0, channels = 0;
    std::vector<unsigned char*> images;
    std::vector<int> heights;

    // Load all textures
    for (auto path : paths) {
        int w, h, c;
        unsigned char* data = stbi_load(path, &w, &h, &c, 0);
        if (!data) {
            throw std::runtime_error(std::string("Failed to load texture: ") + path);
        }
        if (width == 0) {
            width = w;
            channels = c;
        }
        else {
            if (w != width || c != channels) {
                throw std::runtime_error(std::string("All textures must have the same width and channel count to stack vertically. ") + std::to_string(w) + " != " + std::to_string(width) + " or " + std::to_string(c) + " != " + std::to_string(channels) + ". Problematic texture: " + std::string(path));
            }
        }
        images.push_back(data);
        heights.push_back(h);
        totalHeight += h;
    }

    // Allocate a big buffer
    size_t imageSize = width * totalHeight * channels;
    unsigned char* stackedData = new unsigned char[imageSize];

    // Copy images into stackedData at the right offset
    int yOffset = 0;
    for (size_t i = 0; i < images.size(); i++) {
        int h = heights[i];
        size_t rowSize = width * channels;
        for (int row = 0; row < h; row++) {
            memcpy(
                stackedData + ((yOffset + row) * rowSize),
                images[i] + (row * rowSize),
                rowSize
            );
        }
        yOffset += h;
        stbi_image_free(images[i]);
    }

    // Upload to OpenGL
    GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, totalHeight, 0, format, GL_UNSIGNED_BYTE, stackedData);
    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] stackedData;
}