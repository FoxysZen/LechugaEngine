#pragma once
#include <cstdlib>
#include <EmitterShape.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <Logger.h>
#include <Particle.h>
#include <ParticleRenderMode.h>
#include <ParticleType.h>
#include <ShaderProgram.h>
#include <Texture.h>
#include <vector>

class ParticleSystem
{
    public:
        ParticleSystem(ShaderProgram *_shader, ParticleType _type, 
            ParticleRenderMode _mode, EmitterShape _shape, int _maxParticles);
        ~ParticleSystem();

        void init();
        void emit(Particle *particle);
        void update(float deltaTime);
        void draw(const glm::mat4 &view, const glm::mat4 &proj);

        void setDirection(const glm::vec3 &_direction);
        void setPosition(const glm::vec3 &_position);
        void setVelocity(float _velocity);
        void setLifeTime(float _lifeTime);
        void setSpread(float _spread);
        void setColor(const glm::vec3 &_color, const glm::vec3 &_endColor, 
            float _colorCurve);
        void setSize(float _startSize, float _endSize, float _sizeCurve);
        void setEmissionRate(float _rate);
        void setSpiralSpeed(float _spiralSpeed);
        void setGravity(float _gravity);
        void setTexture(Texture *_texture);
        
        float getBoundsRadius();
        glm::vec3 getBoundsCenter();

    private:
        float rand01() { return (float)rand() / RAND_MAX; }

        GLuint VAO, VBO;
        GLuint instanceVBO;
        ShaderProgram *shader;

        const float PI = glm::pi<float>();
        float spiralAngle = 0;

        std::vector<Particle> particles;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 startColor;
        glm::vec3 endColor;
        float velocity;
        float lifeTime;
        float startSize;
        float endSize;
        float sizeCurve;
        float colorCurve;
        float spread;
        float emissionAccumulator;
        float spiralSpeed;
        float gravity;
        int emissionRate;
        int maxParticles;

        ParticleType type;
        ParticleRenderMode renderMode = ParticleRenderMode::COLOR;
        EmitterShape shape;

        Texture *texture = nullptr;
};