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
    for (auto &[id, body] : bodies)
    {
        if (!body->getIsKinematic())
        {
            if (body->getUseGravity())
            {
                body->applyForce(glm::vec3(0.0f, -gravity, 0.0f));
            }
            body->setVelocity(body->getVelocity() + 
                              body->getAcceleration() * deltaTime);

            TransformComponent *transform = scene->getTransform(id);
            transform->position += body->getVelocity() * deltaTime;

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
        TransformComponent *trans1 = scene->getTransform(it1->first);
        SphereCollider *sphere1 = nullptr;
        BoxCollider *box1 = nullptr;
        CapsuleCollider *capsule1 = nullptr;
        MeshCollider *mesh1 = nullptr;
        if (it1->second->getType() == ColliderType::SPHERE)
        {
            sphere1 = dynamic_cast<SphereCollider*>(it1->second);
        }
        if (it1->second->getType() == ColliderType::BOX)
        {
            box1 = dynamic_cast<BoxCollider*>(it1->second);
        }
        if (it1->second->getType() == ColliderType::CAPSULE)
        {
            capsule1 = dynamic_cast<CapsuleCollider*>(it1->second);
        }
        if (it1->second->getType() == ColliderType::MESH)
        {
            mesh1 = dynamic_cast<MeshCollider*>(it1->second);
        }

        auto it2 = it1;
        ++it2;
        for (; it2 != colliders.end(); ++it2)
        {
            TransformComponent *trans2 = scene->getTransform(it2->first);

            ColliderType type1 = it1->second->getType();
            ColliderType type2 = it2->second->getType();

            bool swapped = false;
            if (type1 > type2)
            {
                std::swap(it1, it2);
                std::swap(type1, type2);
                std::swap(trans1, trans2);
                swapped = true;
            }

            if (type1 == ColliderType::SPHERE &&
                type2 == ColliderType::SPHERE)
            {
                SphereCollider *sphere2 = 
                    dynamic_cast<SphereCollider*>(it2->second);

                glm::vec3 pos1 = trans1->position + sphere1->offset;
                glm::vec3 pos2 = trans2->position + sphere2->offset;

                float dist = glm::distance(pos1, pos2);
                if (dist <= sphere1->getRadius() + sphere2->getRadius())
                {
                    CollisionInfo coll = {
                        it1->first, 
                        it2->first,
                        glm::normalize(pos2 - pos1),
                        sphere1->getRadius() + sphere2->getRadius() - dist
                    };
                    info.push_back(coll);
                }
            }
            else if (type1 == ColliderType::SPHERE &&
                     type2 == ColliderType::MESH)
            {
                MeshCollider *mesh2 = 
                    dynamic_cast<MeshCollider*>(it2->second);

                glm::vec3 spherePos = trans1->position + sphere1->offset;
                float radius = sphere1->getRadius();
                auto& triangles = mesh2->getTriangles();

                CollisionInfo bestCollision;
                float maxDepth = 0.0f;
                bool found = false;

                for (int t = 0; t < triangles.size(); t += 3)
                {
                    glm::vec3 closest = closestPointOnTriangle(
                        spherePos,
                        triangles[t],
                        triangles[t+1],
                        triangles[t+2]
                    );
                    float dist = glm::distance(spherePos, closest);
                    if (dist < radius && (radius - dist) > maxDepth)
                    {
                        maxDepth = radius - dist;
                        bestCollision = {
                            it1->first,
                            it2->first,
                            glm::normalize(spherePos - closest),
                            maxDepth
                        };
                        found = true;
                    }
                }

                if (found)
                {
                    info.push_back(bestCollision);
                }
            }
            else if (type1 == ColliderType::CAPSULE &&
                     type2 == ColliderType::MESH)
            {
                glm::vec3 pos = trans1->position + capsule1->offset;
                glm::vec3 top = pos + glm::vec3(0,
                                            capsule1->getHeight() / 2.0f, 0);
                glm::vec3 bottom = pos - glm::vec3(0,
                                            capsule1->getHeight() / 2.0f, 0);
                float radius = capsule1->getRadius();
                auto& triangles = 
                    dynamic_cast<MeshCollider*>(it2->second)->getTriangles();

                CollisionInfo bestCollision;
                float maxDepth = 0.0f;
                bool found = false;

                for (int t = 0; t < triangles.size(); t += 3)
                {
                    glm::vec3 closestOnTri = closestPointOnTriangle(
                        closestPointOnSegment(top, bottom, 
                        (triangles[t] + triangles[t+1] + triangles[t+2]) / 3.0f),
                        triangles[t], triangles[t+1], triangles[t+2]);
                        
                    glm::vec3 closestOnSeg = closestPointOnSegment(top, 
                                                        bottom, closestOnTri);
                    float dist = glm::distance(closestOnSeg, closestOnTri);
                        
                    if (dist < radius && (radius - dist) > maxDepth)
                    {
                        maxDepth = radius - dist;
                        bestCollision = {
                            it1->first,
                            it2->first,
                            glm::normalize(closestOnSeg - closestOnTri),
                            maxDepth
                        };
                        found = true;
                    }
                }

                if (found)
                {
                    info.push_back(bestCollision);
                }
            }
            else if (type1 == ColliderType::CAPSULE && 
                     type2 == ColliderType::CAPSULE)
            {
                CapsuleCollider* capsule2 = 
                    dynamic_cast<CapsuleCollider*>(it2->second);

                glm::vec3 pos1 = trans1->position + capsule1->offset;
                float halfH1 = capsule1->getHeight() / 2.0f;
                glm::vec3 top1 = pos1 + glm::vec3(0.0f, halfH1, 0.0f);
                glm::vec3 bot1 = pos1 - glm::vec3(0.0f, halfH1, 0.0f);

                glm::vec3 pos2 = trans2->position + capsule2->offset;
                float halfH2 = capsule2->getHeight() / 2.0f;
                glm::vec3 top2 = pos2 + glm::vec3(0.0f, halfH2, 0.0f);
                glm::vec3 bot2 = pos2 - glm::vec3(0.0f, halfH2, 0.0f);

                glm::vec3 closest1, closest2;
                closestPointsBetweenSegments(bot1, top1, bot2, top2, closest1, 
                                             closest2);

                float dist = glm::distance(closest1, closest2);
                float sumRadius = 
                    capsule1->getRadius() + capsule2->getRadius();

                if (dist <= sumRadius)
                {
                    glm::vec3 normal;
                    if (dist > 0.0f)
                    {
                        normal = glm::normalize(closest2 - closest1);
                    }
                    else
                    {
                        normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    }
                
                    CollisionInfo coll = {
                        it1->first,
                        it2->first,
                        normal,
                        sumRadius - dist
                    };
                    info.push_back(coll);
                }
            }
            else if (type1 == ColliderType::BOX && 
                     type2 == ColliderType::CAPSULE)
            {
                CapsuleCollider* capsule2 = 
                    dynamic_cast<CapsuleCollider*>(it2->second);

                glm::vec3 capPos = trans2->position + capsule2->offset;
                float halfH = capsule2->getHeight() / 2.0f;
                glm::vec3 top = capPos + glm::vec3(0.0f, halfH, 0.0f);
                glm::vec3 bot = capPos - glm::vec3(0.0f, halfH, 0.0f);

                glm::vec3 boxPos = trans1->position + box1->offset;
                glm::vec3 extents = box1->getHalfExtents();
                glm::vec3 boxMin = boxPos - extents;
                glm::vec3 boxMax = boxPos + extents;

                glm::vec3 closestOnSegment = bot;
                glm::vec3 closestOnBox = glm::clamp(bot, boxMin, boxMax);
                glm::vec3 diff = closestOnSegment - closestOnBox;
                float minSubDistSq = glm::dot(diff, diff);

                glm::vec3 target = boxPos; 
                glm::vec3 ab = top - bot;
                float t = glm::dot(target - bot, ab) / glm::dot(ab, ab);
                t = glm::clamp(t, 0.0f, 1.0f);

                closestOnSegment = bot + t * ab;
                closestOnBox = glm::clamp(closestOnSegment, boxMin, boxMax);

                float dist = glm::distance(closestOnSegment, closestOnBox);
                float radius = capsule2->getRadius();

                if (dist <= radius)
                {
                    glm::vec3 normal;
                    if (dist > 0.0f)
                    {
                        normal = 
                            glm::normalize(closestOnSegment - closestOnBox);
                    }
                    else
                    {
                        normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    }
                
                    CollisionInfo coll = {
                        it1->first,
                        it2->first,
                        normal,
                        radius - dist
                    };
                    info.push_back(coll);
                }
            }

            if (swapped)
            {
                std::swap(it1, it2);
                std::swap(type1, type2);
                std::swap(trans1, trans2);
            }
        }

        sphere1 = nullptr;
        box1 = nullptr;
        capsule1 = nullptr;
        mesh1 = nullptr;
    }

    return info;
}

void PhysicsEngine::resolveCollision(Scene* scene, CollisionInfo info)
{
    RigidBody* body1 = 
        bodies.count(info.entity1) ? bodies[info.entity1] : nullptr;
    RigidBody* body2 = 
        bodies.count(info.entity2) ? bodies[info.entity2] : nullptr;

    TransformComponent* trans1 = scene->getTransform(info.entity1);
    TransformComponent* trans2 = scene->getTransform(info.entity2);

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
        trans2->position += info.normal * (info.depth * separationPercent);
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

glm::vec3 closestPointOnTriangle(glm::vec3 P, glm::vec3 A, glm::vec3 B, 
                                 glm::vec3 C)
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

glm::vec3 closestPointOnSegment(glm::vec3 A, glm::vec3 B, glm::vec3 P)
{
    glm::vec3 AB = B - A;
    float t = glm::dot(P - A, AB) / glm::dot(AB, AB);
    t = glm::clamp(t, 0.0f, 1.0f);
    return A + t * AB;
}

void closestPointsBetweenSegments(glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, 
                                  glm::vec3 q2, glm::vec3 &c1, glm::vec3 &c2)
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
