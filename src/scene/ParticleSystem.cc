#include <ParticleSystem.h>

ParticleSystem::ParticleSystem(ShaderProgram *_shader, ParticleType _type, 
            ParticleRenderMode _mode, EmitterShape _shape, int _maxParticles)
{
    shader = _shader;
    type = _type;
    maxParticles = _maxParticles;
    renderMode = _mode;
    shape = _shape;
}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::init()
{
    emissionAccumulator = 0.0f;
    particles.resize(maxParticles);
    for (auto& p : particles)
        p.active = false;

    // Billboard with UVs
    float quad[] = {
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceVBO);
    glBindVertexArray(VAO);

    // Quad
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // Instancing
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec4) * 2, nullptr, GL_DYNAMIC_DRAW);
    // Position + size
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    // Color
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, (void*)sizeof(glm::vec4));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
}

void ParticleSystem::emit(Particle *particle)
{
    particle->active = true;
    particle->color = startColor;
    particle->size = startSize;
    particle->lifeTime = lifeTime;

    switch (shape)
    {
    case EmitterShape::POINT:
        particle->position = position + glm::vec3(
            (rand01() - 0.5f) * spread,
            (rand01() - 0.5f) * spread,
            (rand01() - 0.5f) * spread
        );
        particle->velocity = direction * velocity;
        break;
    case EmitterShape::FOUNTAIN:
        particle->position = position;
        particle->velocity = glm::vec3(
            (rand01() - 0.5f) * spread,
            velocity,
            (rand01() - 0.5f) * spread
        );
        break;
    case EmitterShape::CIRCLE:
    {
        float angle = rand01() * 2 * PI;
        particle->position = position + glm::vec3(cos(angle) * spread, 0.0f, 
            sin(angle) * spread);
        particle->velocity = direction * velocity;
        break;
    }
    case EmitterShape::SPIRAL:
    {
        spiralAngle += 0.3f;
        particle->position = position + glm::vec3(cos(spiralAngle) * spread, 
            0.0f, sin(spiralAngle) * spread);
        particle->velocity = direction * velocity + 
            glm::vec3(-sin(spiralAngle), 0.0f, cos(spiralAngle)) * spiralSpeed;
        break;
    }
    }
}

void ParticleSystem::update(float deltaTime)
{
    emissionAccumulator += deltaTime;
    float emissionInterval = 1.0f / emissionRate;
    while (emissionAccumulator >= emissionInterval)
    {
        for (auto& p : particles)
        {
            if (!p.active)
            {
                emit(&p);
                break;
            }
        }
        emissionAccumulator -= emissionInterval;
    }

    for (Particle& part : particles)
    {
        if (part.active)
        {
            part.position += part.velocity * deltaTime;
            if (shape == EmitterShape::FOUNTAIN || 
                shape == EmitterShape::CIRCLE || 
                shape == EmitterShape::SPIRAL)
            {
                part.velocity.y -= gravity * deltaTime;
            }
            part.lifeTime -= deltaTime;

            if (part.lifeTime <= 0.0f)
            {
                part.active = false;
                continue;
            }

            float t = 1.0f - (part.lifeTime / lifeTime);
            part.size = glm::mix(startSize, endSize, t);

            float curvedT = pow(t, colorCurve);
            part.color = glm::mix(startColor, endColor, curvedT);
        }
    }
}

void ParticleSystem::draw(const glm::mat4& view, const glm::mat4& proj)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::vector<glm::vec4> instanceData;
    for (auto& p : particles)
    {
        if (p.active)
        {
            instanceData.push_back(glm::vec4(p.position, p.size));
            instanceData.push_back(glm::vec4(p.color, 1.0f));
        }
    }

    if (instanceData.empty()) return;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
        instanceData.size() * sizeof(glm::vec4), instanceData.data());
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, 
        (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, 
        (void*)sizeof(glm::vec4));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    shader->bind();
    shader->setUniformMat4("view", view);
    shader->setUniformMat4("proj", proj);
    shader->setUniformVec3("color", startColor);
    shader->setUniformInt("useTexture", 
        renderMode == ParticleRenderMode::TEXTURE ? 1 : 0);
    if (renderMode == ParticleRenderMode::TEXTURE && texture)
    {
        shader->setUniformInt("particleTexture", 0);
        texture->bind(0);
    }

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceData.size() / 2);
    shader->unbind();

    glDisable(GL_BLEND);
}

void ParticleSystem::setDirection(const glm::vec3 &_direction)
{
    direction = _direction;
}

void ParticleSystem::setPosition(const glm::vec3 &_position)
{
    position = _position;
}

void ParticleSystem::setVelocity(float _velocity)
{
    velocity = _velocity;
}

void ParticleSystem::setLifeTime(float _lifeTime)
{
    lifeTime = _lifeTime;
}

void ParticleSystem::setSpread(float _spread)
{
    spread = _spread;
}

void ParticleSystem::setColor(const glm::vec3 &_startColor, 
                              const glm::vec3 &_endColor, float _colorCurve)
{
    startColor = _startColor;
    endColor = _endColor;
    colorCurve = _colorCurve;
}

void ParticleSystem::setSize(float _startSize, float _endSize, 
                             float _sizeCurve)
{
    startSize = _startSize;
    endSize = _endSize;
    sizeCurve = _sizeCurve;
}

void ParticleSystem::setEmissionRate(float _rate)
{
    emissionRate = _rate;
}

void ParticleSystem::setSpiralSpeed(float _spiralSpeed)
{
    spiralSpeed = _spiralSpeed;
}

void ParticleSystem::setGravity(float _gravity)
{
    gravity = _gravity;
}

void ParticleSystem::setTexture(Texture* _texture)
{
    texture = _texture;
    renderMode = ParticleRenderMode::TEXTURE;
}

float ParticleSystem::getBoundsRadius()
{
    return (spread / 2.0f) + velocity * lifeTime;
}

glm::vec3 ParticleSystem::getBoundsCenter()
{
    return position + direction * (velocity * lifeTime / 2.0f);
}
