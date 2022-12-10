#pragma once
#include "ShaderProgram/ShaderProgram.hpp"
#include <memory>
#include <array>
#include "camera.hpp"
#include "texture.hpp"

class Skybox
{
    public:
        Skybox();
        void shutdown();
        void draw(int width, int height);

    private:
        std::unique_ptr<ShaderProgram> skyboxShader;
        GLuint skyboxVAO;
        Texture skyboxTexture;
};