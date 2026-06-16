#include <PhysicsEngine.h>

PhysicsEngine::PhysicsEngine() {}

PhysicsEngine::~PhysicsEngine() {}

void PhysicsEngine::addBody(EntityID id, RigidBody *body)
{
    bodies[id] = body;
}

void PhysicsEngine::addCollider(EntityID id, Collider *collider)
{
    colliders[id] = collider;
}

void PhysicsEngine::step(float deltaTime, Scene* scene)
{
    Logger::info("Bodies count: " + std::to_string(bodies.size()));
    for (auto &[id, body] : bodies)
    {
        Logger::info("Body id: " + std::to_string(id) + 
                 " useGravity: " + std::to_string(body->getUseGravity()) +
                 " isKinematic: " + std::to_string(body->getIsKinematic()));
        if (body->getMass() <= 0.0f) continue;
        if (!body->getIsKinematic())
        {
            if (body->getUseGravity())
            {
                body->applyForce(glm::vec3(0.0f, -gravity, 0.0f));
            }
            body->setVelocity(body->getVelocity() + 
                              body->getAcceleration() * deltaTime);
Logger::info("velocity: " + std::to_string(body->getVelocity().y));
Logger::info("acceleration: " + std::to_string(body->getAcceleration().y));

            TransformComponent* transform = scene->getTransform(id);
            // Terrain and other may not have Transform
            if (transform == nullptr)
            {
                continue;
            }
            Logger::info("pos before: " + std::to_string(transform->position.y));
            transform->position += body->getVelocity() * deltaTime;
            Logger::info("pos after: " + std::to_string(transform->position.y));

            body->setAcceleration(glm::vec3(0.0f));
        }
    }

    std::vector<CollisionInfo> collisions = detectCollisions(scene);
    for (CollisionInfo info : collisions)
    {
        resolveCollision(scene, info);
    }
}

std::vector<CollisionInfo> PhysicsEngine::detectCollisions(Scene* scene)
{
    std::vector<CollisionInfo> info;

    for (auto it1 = colliders.begin(); it1 != colliders.end(); ++it1)
    {
        auto it2 = it1;
        ++it2;
        for (; it2 != colliders.end(); ++it2)
        {
            auto* cur1 = &(*it1);
            auto* cur2 = &(*it2);
            TransformComponent* curTrans1 = scene->getTransform(cur1->first);
            TransformComponent* curTrans2 = scene->getTransform(cur2->first);
            ColliderType type1 = cur1->second->getType();
            ColliderType type2 = cur2->second->getType();

            if (type1 > type2)
            {
                std::swap(cur1, cur2);
                std::swap(curTrans1, curTrans2);
                std::swap(type1, type2);
            }

            SphereCollider* sphere1 = (type1 == ColliderType::SPHERE) ? 
                dynamic_cast<SphereCollider*>(cur1->second) : nullptr;
            BoxCollider* box1 = (type1 == ColliderType::BOX) ? 
                dynamic_cast<BoxCollider*>(cur1->second) : nullptr;
            CapsuleCollider* capsule1 = (type1 == ColliderType::CAPSULE) ? 
                dynamic_cast<CapsuleCollider*>(cur1->second) : nullptr;

            if (type1 == ColliderType::SPHERE && type2 == ColliderType::SPHERE)
            {
                SphereCollider* sphere2 = 
                    dynamic_cast<SphereCollider*>(cur2->second);
                glm::vec3 pos1 = curTrans1->position + sphere1->offset;
                glm::vec3 pos2 = curTrans2->position + sphere2->offset;
                float dist = glm::distance(pos1, pos2);
                if (dist <= sphere1->getRadius() + sphere2->getRadius())
                {
                    glm::vec3 diff = pos2 - pos1;
                    if (glm::length(diff) < 0.0001f)
                        continue;
                    info.push_back({cur1->first, cur2->first,
                        glm::normalize(pos2 - pos1),
                        sphere1->getRadius() + sphere2->getRadius() - dist});
                }
            }
            else if (type1 == ColliderType::SPHERE && 
                     type2 == ColliderType::MESH)
            {
                MeshCollider* mesh2 = dynamic_cast<MeshCollider*>(cur2->second);
                glm::vec3 spherePos = curTrans1->position + sphere1->offset;
                float radius = sphere1->getRadius();
                auto& triangles = mesh2->getTriangles();
                CollisionInfo bestCollision;
                float maxDepth = 0.0f;
                bool found = false;
                for (size_t t = 0; t < triangles.size(); t += 3)
                {
                    glm::vec3 closest = closestPointOnTriangle(spherePos,
                        triangles[t], triangles[t+1], triangles[t+2]);
                    float dist = glm::distance(spherePos, closest);
                    if (dist < radius && (radius - dist) > maxDepth)
                    {
                        glm::vec3 diff = spherePos - closest;
                        if (glm::length(diff) < 0.0001f) continue;
                        maxDepth = radius - dist;
                        bestCollision = {cur1->first, cur2->first,
                            glm::normalize(spherePos - closest), maxDepth};
                        found = true;
                    }
                }
                if (found) info.push_back(bestCollision);
            }
            else if (type1 == ColliderType::CAPSULE && 
                     type2 == ColliderType::MESH)
            {
                MeshCollider* mesh2 = dynamic_cast<MeshCollider*>(cur2->second);
                glm::vec3 pos = curTrans1->position + capsule1->offset;
                glm::vec3 top = pos + glm::vec3(0, 
                                            capsule1->getHeight() / 2.0f, 0);
                glm::vec3 bottom = pos - glm::vec3(0, 
                                            capsule1->getHeight() / 2.0f, 0);
                float radius = capsule1->getRadius();
                auto& triangles = mesh2->getTriangles();
                CollisionInfo bestCollision;
                float maxDepth = 0.0f;
                bool found = false;
                for (size_t t = 0; t < triangles.size(); t += 3)
                {
                    glm::vec3 closestOnTri = closestPointOnTriangle(
                        closestPointOnSegment(top, bottom,
                            (triangles[t] + triangles[t+1] + triangles[t+2]) / 3.0f),
                        triangles[t], triangles[t+1], triangles[t+2]);
                    glm::vec3 closestOnSeg = 
                            closestPointOnSegment(top, bottom, closestOnTri);
                    glm::vec3 diff = closestOnSeg - closestOnTri;
                    if (glm::length(diff) < 0.0001f) // Avoids NaN
                        continue;
                    float dist = glm::distance(closestOnSeg, closestOnTri);
                    if (dist < radius && (radius - dist) > maxDepth)
                    {
                        maxDepth = radius - dist;
                        bestCollision = {cur1->first, cur2->first,
                            glm::normalize(closestOnSeg - closestOnTri), maxDepth};
                        found = true;
                    }
                }
                if (found) info.push_back(bestCollision);
            }
            else if (type1 == ColliderType::CAPSULE && 
                     type2 == ColliderType::CAPSULE)
            {
                CapsuleCollider* capsule2 = 
                    dynamic_cast<CapsuleCollider*>(cur2->second);
                glm::vec3 pos1 = curTrans1->position + capsule1->offset;
                float halfH1 = capsule1->getHeight() / 2.0f;
                glm::vec3 top1 = pos1 + glm::vec3(0.0f, halfH1, 0.0f);
                glm::vec3 bot1 = pos1 - glm::vec3(0.0f, halfH1, 0.0f);
                glm::vec3 pos2 = curTrans2->position + capsule2->offset;
                float halfH2 = capsule2->getHeight() / 2.0f;
                glm::vec3 top2 = pos2 + glm::vec3(0.0f, halfH2, 0.0f);
                glm::vec3 bot2 = pos2 - glm::vec3(0.0f, halfH2, 0.0f);
                glm::vec3 closest1, closest2;
                closestPointsBetweenSegments(bot1, top1, bot2, top2, closest1, closest2);
                float dist = glm::distance(closest1, closest2);
                float sumRadius = capsule1->getRadius() + capsule2->getRadius();
                if (dist <= sumRadius)
                {
                    glm::vec3 diff = closest2 - closest1;
                    glm::vec3 normal = glm::length(diff) > 0.0001f ? 
                        glm::normalize(diff) : glm::vec3(0.0f, 1.0f, 0.0f);
                    info.push_back({cur1->first, cur2->first, normal, sumRadius - dist});
                }
            }
            else if (type1 == ColliderType::BOX && 
                     type2 == ColliderType::CAPSULE)
            {
                CapsuleCollider* capsule2 = 
                    dynamic_cast<CapsuleCollider*>(cur2->second);
                glm::vec3 capPos = curTrans2->position + capsule2->offset;
                float halfH = capsule2->getHeight() / 2.0f;
                glm::vec3 top = capPos + glm::vec3(0.0f, halfH, 0.0f);
                glm::vec3 bot = capPos - glm::vec3(0.0f, halfH, 0.0f);
                glm::vec3 boxPos = curTrans1->position + box1->offset;
                glm::vec3 extents = box1->getHalfExtents();
                glm::vec3 boxMin = boxPos - extents;
                glm::vec3 boxMax = boxPos + extents;
                glm::vec3 ab = top - bot;
                float t = glm::dot(boxPos - bot, ab) / glm::dot(ab, ab);
                t = glm::clamp(t, 0.0f, 1.0f);
                glm::vec3 closestOnSeg = bot + t * ab;
                glm::vec3 closestOnBox = glm::clamp(closestOnSeg, boxMin, boxMax);
                float dist = glm::distance(closestOnSeg, closestOnBox);
                float radius = capsule2->getRadius();

                if (dist <= radius)
                {
                    glm::vec3 diff = closestOnSeg - closestOnBox;
                    glm::vec3 normal = glm::length(diff) > 0.0001f ? 
                        glm::normalize(diff) : glm::vec3(0.0f, 1.0f, 0.0f);
                    info.push_back({cur1->first, cur2->first, normal, radius - dist});
                }
            }
        }
    }
Logger::info("Collisions detected: " + std::to_string(info.size()));
    return info;
}

void PhysicsEngine::resolveCollision(Scene* scene, CollisionInfo info)
{
    Logger::info("resolving: entity1=" + std::to_string(info.entity1) + 
             " entity2=" + std::to_string(info.entity2));
    Logger::info("normal: " + std::to_string(info.normal.x) + " " + 
             std::to_string(info.normal.y) + " " + 
             std::to_string(info.normal.z) +
             " depth: " + std::to_string(info.depth));

    RigidBody* body1 = 
        bodies.count(info.entity1) ? bodies[info.entity1] : nullptr;
    RigidBody* body2 = 
        bodies.count(info.entity2) ? bodies[info.entity2] : nullptr;

    TransformComponent* trans1 = scene->getTransform(info.entity1);
    TransformComponent* curTrans2 = scene->getTransform(info.entity2);
    if (trans1 == nullptr || curTrans2 == nullptr)
    {
        return;
    }

    bool isStatic1 = !body1 || body1->getIsKinematic();
    bool isStatic2 = !body2 || body2->getIsKinematic();

    if (isStatic1 && isStatic2) return;

    float totalMass = (isStatic1 ? 0.0f : 1.0f) + (isStatic2 ? 0.0f : 1.0f); 
    float separationPercent = 1.0f / totalMass; 

    if (!isStatic1)
    {
        trans1->position -= info.normal * (info.depth * separationPercent);
    }
    if (!isStatic2)
    {
        curTrans2->position += info.normal * (info.depth * separationPercent);
    }

    if (body1 || body2)
    {
        glm::vec3 vel1 = body1 ? body1->getVelocity() : glm::vec3(0.0f);
        glm::vec3 vel2 = body2 ? body2->getVelocity() : glm::vec3(0.0f);

        glm::vec3 relativeVelocity = vel2 - vel1;
        float velAlongNormal = glm::dot(relativeVelocity, info.normal);

        if (velAlongNormal < 0.0f)
        {
            float restitution = 0.2f; 

            float impulseScalar = -(1.0f + restitution) * velAlongNormal;
            impulseScalar /= totalMass;

            if (!isStatic1)
            {
                body1->setVelocity(body1->getVelocity() - impulseScalar * info.normal);
            }
            if (!isStatic2)
            {
                body2->setVelocity(body2->getVelocity() + impulseScalar * info.normal);
            }
        }
    }
}

glm::vec3 PhysicsEngine::closestPointOnTriangle(glm::vec3 P, glm::vec3 A, 
    glm::vec3 B, glm::vec3 C)
{
    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;
    glm::vec3 AP = P - A;

    float d1 = glm::dot(AB, AP);
    float d2 = glm::dot(AC, AP);
    if (d1 <= 0.0f && d2 <= 0.0f) return A;

    glm::vec3 BP = P - B;
    float d3 = glm::dot(AB, BP);
    float d4 = glm::dot(AC, BP);
    if (d3 >= 0.0f && d4 <= d3) return B;

    glm::vec3 CP = P - C;
    float d5 = glm::dot(AB, CP);
    float d6 = glm::dot(AC, CP);
    if (d6 >= 0.0f && d5 <= d6) return C;

    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
    {
        float v = d1 / (d1 - d3);
        return A + v * AB; // Side AB
    }

    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
    {
        float w = d2 / (d2 - d6);
        return A + w * AC; // Side AC
    }

    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
    {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return B + w * (C - B); // Side BC
    }

    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return A + v * AB + w * AC; // Inside triangle
}

glm::vec3 PhysicsEngine::closestPointOnSegment(glm::vec3 A, glm::vec3 B, 
    glm::vec3 P)
{
    glm::vec3 AB = B - A;
    float t = glm::dot(P - A, AB) / glm::dot(AB, AB);
    t = glm::clamp(t, 0.0f, 1.0f);
    return A + t * AB;
}

void PhysicsEngine::closestPointsBetweenSegments(glm::vec3 p1, glm::vec3 q1, 
    glm::vec3 p2, glm::vec3 q2, glm::vec3 &c1, glm::vec3 &c2)
{
    glm::vec3 d1 = q1 - p1; 
    glm::vec3 d2 = q2 - p2; 
    glm::vec3 r = p1 - p2;
    float a = glm::dot(d1, d1); 
    float e = glm::dot(d2, d2); 
    float f = glm::dot(d2, r);

    float s = 0.0f, t = 0.0f;

    if (a <= 0.0f && e <= 0.0f) {
        c1 = p1; c2 = p2; return;
    }
    if (a <= 0.0f) {
        s = 0.0f;
        t = glm::clamp(f / e, 0.0f, 1.0f);
    } else {
        float c = glm::dot(d1, r);
        if (e <= 0.0f) {
            t = 0.0f;
            s = glm::clamp(-c / a, 0.0f, 1.0f);
        } else {
            float b = glm::dot(d1, d2);
            float denom = a * e - b * b;

            if (denom != 0.0f) {
                s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            } else {
                s = 0.0f;
            }
            t = (b * s + f) / e;

            if (t < 0.0f) {
                t = 0.0f;
                s = glm::clamp(-c / a, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = glm::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
}
