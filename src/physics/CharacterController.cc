#include <CharacterController.h>

CharacterController::CharacterController(EntityID _id, PhysicsEngine *_physics, 
                                         Scene *_scene)
{
    id = _id;
    physics = _physics;
    scene = _scene;
}

CharacterController::~CharacterController() {}

void CharacterController::move(glm::vec3 direction, float speed, float deltaTime)
{
    TransformComponent *transform = scene->getTransform(id);

    if (glm::length(direction) > 0.001f)
    {
        float targetAngle = glm::degrees(std::atan2(direction.x, direction.z));
        float rotationSpeed = 10.0f;
        float diff = targetAngle - transform->rotation.y;
        while (diff < -180.0f) diff += 360.0f;
        while (diff >  180.0f) diff -= 360.0f;
        transform->rotation.y += diff * rotationSpeed * deltaTime;
    }

    if (!grounded)
        velocity.y -= gravity * deltaTime;
    else
        velocity.y = 0.0f;

    velocity.x = direction.x * speed;
    velocity.z = direction.z * speed;

    transform->position += velocity * deltaTime;
    grounded = false;

    const int MAX_ITERATIONS = 3;
    for (int iter = 0; iter < MAX_ITERATIONS; iter++)
    {
        std::vector<CollisionInfo> collisions;
        physics->getCollisionsFor(id, scene, collisions);
        if (collisions.empty()) break;

        for (auto &col : collisions)
        {
            glm::vec3 effectiveNormal = (col.entity1 == id) ? col.normal : -col.normal;
            float slop = 0.001f;
            float correction = std::max(col.depth - slop, 0.0f);
        
            glm::vec3 correctionVec = effectiveNormal * correction;
            transform->position += correctionVec;
        
            float velAlongNormal = glm::dot(velocity, effectiveNormal);
            if (velAlongNormal < 0.0f)
            {
                if (effectiveNormal.y > 0.5f)
                {
                    if (velocity.y < 0.0f) 
                        velocity.y = 0.0f;
                }
                else
                {
                    velocity -= effectiveNormal * velAlongNormal;
                }
            }
        
            if (effectiveNormal.y > 0.5f)
            {
                grounded = true;
            }
        }
    }
}
bool CharacterController::isGrounded()
{
    return grounded;
}

void CharacterController::setGrounded(bool g)
{
    grounded = g;
}

EntityID CharacterController::getPlayerId() const
{
    return id;
}
