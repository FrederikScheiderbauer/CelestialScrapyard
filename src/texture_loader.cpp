#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ImageLoader/stb_image.h>

#include "texture_loader.hpp"


bool TextureLoader::generate_texture(std::string texturePath,GLuint* textureID)
{
    glGenTextures(1,textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);

    int width, height, nrChannels;
    bool success;
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        success = true;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        success = false;
    }
    stbi_image_free(data);

    return success;
}
