#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class TextureLoader
{
    public:
        static unsigned int generate_texture(std::string texturepath);
        static unsigned int generate_cubemap(std::vector<std::string> cubemap_texturePaths);
};