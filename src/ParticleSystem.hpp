//based on: https://github.com/TheCherno/OneHourParticleSystem/blob/master/OpenGL-Sandbox/src/ParticleSystem.h

#ifndef GGD_PARTICLESYSTEM_HPP
#define GGD_PARTICLESYSTEM_HPP

#include <memory>
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "ShaderProgram/ShaderProgram.hpp"

struct ParticleProps
{
    glm::vec3 Position, CraterCenter;
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
    ~ParticleSystem();
private:
    struct Particle
    {
        glm::vec3 Position, CraterCenter;
        glm::vec3 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float SizeBegin, SizeEnd;

        int LifeTime;
        int LifeRemaining;

        bool Active = false;
    };
    std::vector<Particle> m_ParticlePool;
    const int numParticles = 10000;
    int m_PoolIndex = numParticles - 1;

    GLuint m_QuadVA = 0;
    //particle infos required for rendering
    GLuint particleInfoBuffer;
    struct ParticleInfo {
        glm::vec4 positionAndSize;
        glm::vec4 color;
    };
    ParticleInfo *particleInfos;

    std::unique_ptr<ShaderProgram> m_ParticleShader;
    void setUniformMatrix(glm::mat4 matrix, std::string type);
};


#endif //GGD_PARTICLESYSTEM_HPP
