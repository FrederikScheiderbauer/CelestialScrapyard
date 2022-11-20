#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ImageLoader/stb_image.h>

#include "texture_loader.hpp"

//https://learnopengl.com/Getting-started/Textures
unsigned int TextureLoader::generate_texture(std::string texturePath)
{
    unsigned int textureID;
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

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
    
    return textureID;
}

unsigned int TextureLoader::generate_cubemap(std::vector<std::string> cubemap_texturePaths)
{
    unsigned int cubemapID;
    glGenTextures(1,&cubemapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < cubemap_texturePaths.size(); i++)
    {
        unsigned char *data = stbi_load(cubemap_texturePaths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << cubemap_texturePaths[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return cubemapID;
}