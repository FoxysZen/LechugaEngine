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
    if (!grounded)
        velocity.y -= gravity * deltaTime;
    else
        velocity.y = 0.0f;

    velocity.x = direction.x * speed;
    velocity.z = direction.z * speed;

    TransformComponent* transform = scene->getTransform(id);
    transform->position += velocity * deltaTime;

    grounded = false;

    std::vector<CollisionInfo> collisions = physics->getCollisionsFor(id, scene);
    for (auto& col : collisions)
    {
        float percent = 0.8f;
        float slop = 0.01f;
        float correction = std::max(col.depth - slop, 0.0f) * percent;

        if (col.entity1 == id)
            transform->position -= col.normal * correction;
        else
            transform->position += col.normal * correction;

        float velAlongNormal = glm::dot(velocity, col.normal);
        if (velAlongNormal < 0.0f)
        {
            if (col.entity1 == id)
                velocity -= col.normal * velAlongNormal;
            else
                velocity += col.normal * velAlongNormal;
        }

        glm::vec3 effectiveNormal = (col.entity1 == id) ? col.normal : -col.normal;
        if (effectiveNormal.y > 0.7f)
        {
            grounded = true;
            velocity.y = 0.0f;
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
