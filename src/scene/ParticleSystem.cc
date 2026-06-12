#include <ParticleSystem.h>

ParticleSystem::ParticleSystem(ShaderProgram *_shader, ParticleType _type, 
            int _maxParticles)
{
    shader = _shader;
    type = _type;
    maxParticles = _maxParticles;
}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::init()
{
    emissionAccumulator = 0.0f;

    particles.resize(maxParticles);
    for (auto& p : particles)
        p.active = false;

    // Billboard
    float quad[] = {
        -0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceVBO);
    
    glBindVertexArray(VAO);
    // Quad
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Instancing
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec4), nullptr, 
        GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);
}

void ParticleSystem::emit(Particle *particle)
{
    particle->active = true;
    particle->position = position + glm::vec3(
        (rand01() - 0.5f) * spread,
        (rand01() - 0.5f) * spread,
        (rand01() - 0.5f) * spread
    );
    particle->color = startColor;
    particle->size = startSize;
    particle->lifeTime = lifeTime;
    particle->velocity = direction * velocity;
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
            part.lifeTime -= deltaTime;
            if (part.lifeTime <= 0.0f) part.active = false;
        }
    }
}

void ParticleSystem::draw(const glm::mat4& view, const glm::mat4& proj)
{
    std::vector<glm::vec4> instanceData;
    for (auto& p : particles)
    {
        if (p.active)
            instanceData.push_back(glm::vec4(p.position, p.size));
    }

    if (instanceData.empty()) return;

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
        instanceData.size() * sizeof(glm::vec4), instanceData.data());

    shader->bind();
    shader->setUniformMat4("view", view);
    shader->setUniformMat4("proj", proj);
    shader->setUniformVec3("color", startColor);

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceData.size());
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

void ParticleSystem::setColor(const glm::vec3 &_color)
{
    startColor = _color;
}

void ParticleSystem::setSize(float _size)
{
    startSize = _size;
}

void ParticleSystem::setEmissionRate(float _rate)
{
    emissionRate = _rate;
}

float ParticleSystem::getBoundsRadius()
{
    return (spread / 2.0f) + velocity * lifeTime;
}

glm::vec3 ParticleSystem::getBoundsCenter()
{
    return position + direction * (velocity * lifeTime / 2.0f);
}
