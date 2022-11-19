#include <string>

class TextureLoader
{
    public:
        static bool generate_texture(std::string texturepath, GLuint* textureID);
};