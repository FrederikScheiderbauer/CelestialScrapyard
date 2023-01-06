#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../headers/texture.hpp"

class TextureLoader
{
    public:
        static Texture generate_diffuse_texture(std::string texturepath);
        static Texture generate_cubemap(std::vector<std::string> cubemap_texturePaths);
        static Texture generate_One_Texture_As_cubemap(std::string texturepath);
};