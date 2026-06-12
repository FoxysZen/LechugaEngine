#pragma once
#include <cstdlib>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <Logger.h>
#include <Particle.h>
#include <ShaderProgram.h>
#include <vector>

enum class ParticleType { BILLBOARD, MESH };

class ParticleSystem
{
    public:
        ParticleSystem(ShaderProgram *_shader, ParticleType _type, 
            int _maxParticles);
        ~ParticleSystem();

        void init(int maxParticles);
        void emit(Particle *particle);
        void update(float deltaTime);
        void draw(const glm::mat4& view, const glm::mat4& proj);

        void setDirection(const glm::vec3 &_direction);
        void setPosition(const glm::vec3 &_position);
        void setVelocity(float _velocity);
        void setLifeTime(float _lifeTime);
        void setSpread(float _spread);
        void setColor(const glm::vec3 &_color);
        void setSize(float _size);
        void setEmissionRate(float _rate);
        
        float getBoundsRadius();
        glm::vec3 getBoundsCenter();

    private:
        float rand01() { return (float)rand() / RAND_MAX; }

        GLuint VAO, VBO;
        GLuint instanceVBO;
        ShaderProgram* shader;

        std::vector<Particle> particles;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 startColor;
        float velocity;
        float lifeTime;
        float startSize;
        float spread;
        float emissionAccumulator;
        int emissionRate;
        int maxParticles;
        ParticleType type;
};