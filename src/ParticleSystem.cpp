//based on: https://github.com/TheCherno/OneHourParticleSystem/blob/master/OpenGL-Sandbox/src/ParticleSystem.cpp
#include "ParticleSystem.hpp"

#include "config/config.h"
#include "camera.hpp"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

const std::vector<std::string> SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/particle.vert", (std::string)Project_SOURCE_DIR + "/src/shader/particle.frag"};

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

ParticleSystem::ParticleSystem()
{
    m_ParticlePool.resize(1000);

    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
    };

    glCreateVertexArrays(1, &m_QuadVA);
    glBindVertexArray(m_QuadVA);

    GLuint quadVB, quadIB;
    glCreateBuffers(1, &quadVB);
    glBindBuffer(GL_ARRAY_BUFFER, quadVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(quadVB, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    uint32_t indices[] = {
            0, 1, 2, 2, 3, 0
    };

    glCreateBuffers(1, &quadIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    m_ParticleShader = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);
    m_ParticleShaderColor = glGetUniformLocation(m_ParticleShader->name, "u_Color");
}

void ParticleSystem::setUniformMatrix(glm::mat4 matrix, std::string type)
{
    glUniformMatrix4fv(glGetUniformLocation(m_ParticleShader->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void ParticleSystem::draw(int width, int height) {
    m_ParticleShader->use();
    Camera* camera = Camera::get_Active_Camera();

    //generating Projection,Model and view matrixes for Shader.
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    setUniformMatrix(proj,"projection");

    glm::mat4 view = glm::mat4(1.0f);

    glm::vec3 cameraPos = camera->get_Camera_Position();
    view = camera->get_View_Matrix();
    setUniformMatrix(view,"view");

    for (auto& particle : m_ParticlePool)
    {
        if (!particle.Active)
            continue;

        if (particle.LifeRemaining <= 0)
        {
            particle.Active = false;
            continue;
        }

        --particle.LifeRemaining;
        particle.Position += particle.Velocity;

        // Fade away particles
        float life = static_cast<float>(particle.LifeRemaining) / static_cast<float>(particle.LifeTime);
        glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
        color.a = color.a * life;

        float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

        // Render
        glm::mat4 model = glm::translate(glm::mat4(1.0f), { particle.Position.x, particle.Position.y, particle.Position.z }) * glm::scale(glm::mat4(1.0f), { size, size, size });
        setUniformMatrix(model,"model");

        glUniform4fv(m_ParticleShaderColor, 1, glm::value_ptr(color));
        glBindVertexArray(m_QuadVA);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
}

void ParticleSystem::emit(const ParticleProps& particleProps)
{
    Particle& particle = m_ParticlePool[m_PoolIndex];
    particle.Active = true;
    particle.Position = particleProps.Position;

    // Velocity
    particle.Velocity = particleProps.Velocity;
    //particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f);
    //particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);

    // Color
    particle.ColorBegin = particleProps.ColorBegin;
    particle.ColorEnd = particleProps.ColorEnd;

    particle.LifeTime = particleProps.LifeTime;
    particle.LifeRemaining = particleProps.LifeTime;
    particle.SizeBegin = particleProps.SizeBegin /*+ particleProps.SizeVariation * (Random::Float() - 0.5f)*/;
    particle.SizeEnd = particleProps.SizeEnd;

    m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}

void ParticleSystem::setAllInactive() {
    for (auto &particle : m_ParticlePool) {
        particle.Active = false;
    }
}
