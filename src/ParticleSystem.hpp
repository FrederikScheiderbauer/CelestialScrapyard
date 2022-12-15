//based on: https://github.com/TheCherno/OneHourParticleSystem/blob/master/OpenGL-Sandbox/src/ParticleSystem.h

#ifndef GGD_PARTICLESYSTEM_HPP
#define GGD_PARTICLESYSTEM_HPP

#include <memory>
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "ShaderProgram/ShaderProgram.hpp"

struct ParticleProps
{
    glm::vec3 Position, CraterNormal;
    glm::vec3 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float SizeBegin, SizeEnd, SizeVariation;
    int LifeTime;
};

class ParticleSystem
{
public:
    ParticleSystem();

    void draw(int width, int height);
    void emit(const ParticleProps& particleProps);
    void setInactiveForCenter(glm::vec3 center);
private:
    struct Particle
    {
        glm::vec3 Position, RotationAxis;
        glm::vec3 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation;
        float SizeBegin, SizeEnd;

        int LifeTime;
        int LifeRemaining;

        bool Active = false;
    };
    std::vector<Particle> m_ParticlePool;
    uint32_t m_PoolIndex = 9999;

    GLuint m_QuadVA = 0;
    std::unique_ptr<ShaderProgram> m_ParticleShader;
    GLint m_ParticleShaderColor;
    GLint ParticleShaderPosition;
    GLint ParticleShaderSize;

    void setUniformMatrix(glm::mat4 matrix, std::string type);
};


#endif //GGD_PARTICLESYSTEM_HPP
