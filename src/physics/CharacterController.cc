#include <CharacterController.h>

CharacterController::CharacterController(EntityID _id, PhysicsEngine *_physics, 
                                         Scene *_scene)
{
    id = _id;
    physics = _physics;
    scene = _scene;
}

CharacterController::~CharacterController() {}

void CharacterController::move(glm::vec3 direction, float speed, 
                               float deltaTime)
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

    const int MAX_ITERATIONS = 3;
    for (int iter = 0; iter < MAX_ITERATIONS; iter++)
    {
        std::vector<CollisionInfo> collisions = 
            physics->getCollisionsFor(id, scene);
        if (collisions.empty()) break;

        for (auto& col : collisions)
        {
            glm::vec3 effectiveNormal = 
                (col.entity1 == id) ? col.normal : -col.normal;
            
            float slop = 0.001f;
            float correction = std::max(col.depth - slop, 0.0f);
            transform->position += effectiveNormal * correction;

            float velAlongNormal = glm::dot(velocity, effectiveNormal);
            if (velAlongNormal < 0.0f)
                velocity -= effectiveNormal * velAlongNormal;

            if (effectiveNormal.y > 0.5f)
            {
                grounded = true;
                velocity.y = 0.0f;
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
