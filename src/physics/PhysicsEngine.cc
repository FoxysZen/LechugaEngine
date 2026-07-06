#include <ColliderType.h>
#include <PhysicsEngine.h>
#include <cstddef>
#include <cstdint>

PhysicsEngine::PhysicsEngine() {}

PhysicsEngine::~PhysicsEngine() {}

void PhysicsEngine::addBody(EntityID id, RigidBody *body)
{
    if (id >= bodies.size())
    {
        bodies.resize(id + 1, nullptr);
    }
    
    bodies[id] = body;

    if (!body->getIsKinematic())
    {
        dynamicObjects.push_back(id);
    }
}

void PhysicsEngine::addCollider(EntityID id, Collider *collider)
{
    colliders[id] = collider;

    if (collider->getType() == ColliderType::MESH)
    {
        staticObjects.push_back(id);

        // AABB infinite grid
        MeshCollider *meshCollider = static_cast<MeshCollider*>(collider);
        auto &tris = meshCollider->getTriangles();
        size_t size = tris.size();
        if (size % 3 != 0)
        {
            Logger::error("PhysicsEngine: The triangles vector is incomplete or wrong");
            return;
        }

        for (size_t i = 0; i < size; i = i + 3)
        {
            float minX = std::min(tris[i].x, std::min(tris[i + 1].x,
                                                      tris[i + 2].x));
            float minZ = std::min(tris[i].z, std::min(tris[i + 1].z,
                                                      tris[i + 2].z));
            float maxX = std::max(tris[i].x, std::max(tris[i + 1].x,
                                                      tris[i + 2].x));
            float maxZ = std::max(tris[i].z, std::max(tris[i + 1].z,
                                                      tris[i + 2].z));

            int minCellX = std::floor(minX / cellSize);
            int minCellZ = std::floor(minZ / cellSize);
            int maxCellX = std::floor(maxX / cellSize);
            int maxCellZ = std::floor(maxZ / cellSize);

            for (int x = minCellX; x <= maxCellX; ++x)
            {
                uint64_t hashX = static_cast<uint64_t>(x) << 32;
                for (int z = minCellZ; z <= maxCellZ; ++z)
                {
                    uint64_t hash = hashX | (static_cast<uint64_t>(z) & 0xFFFFFFFF);

                    cells[hash].trisIndex.push_back({id, i});
                }
            }
        }
    }
}

void PhysicsEngine::step(float deltaTime, Scene *scene)
{
    for (EntityID id = 0; id < bodies.size(); ++id)
    {
        RigidBody *body = bodies[id];
        if (!body) continue;

        if (body->getMass() <= 0.0f) continue;
        if (!body->getIsKinematic())
        {
            if (body->getUseGravity() && body->getIsGrounded())
            {
                body->setAcceleration(glm::vec3(0.0f));
                if (body->getVelocity().y < 0.0f) 
                {
                    body->setVelocity(glm::vec3(body->getVelocity().x, 0.0f, 
                                                body->getVelocity().z));
                }
            }
            else if (body->getUseGravity())
            {
                body->setAcceleration(glm::vec3(0.0f, -gravity, 0.0f));
            }

            body->setVelocity(body->getVelocity() + body->getAcceleration() * deltaTime);
            
            TransformComponent *transform = scene->getTransform(id);
            if (transform) 
            {
                transform->position += body->getVelocity() * deltaTime;
            }

            body->setAcceleration(glm::vec3(0.0f));
        }

        body->setIsGrounded(false);
    }

    collisions.clear();
    detectCollisions(scene, collisions);
    for (CollisionInfo &info : collisions)
    {
        resolveCollision(scene, info);
    }
}

void PhysicsEngine::detectCollisions(Scene *scene, 
                                     std::vector<CollisionInfo> &info)
{
    for (size_t i = 0; i < dynamicObjects.size(); ++i)
    {
        EntityID id1 = dynamicObjects[i];
        Collider *col1 = colliders[id1];
        TransformComponent *trans1 = scene->getTransform(id1);
        
        if (!col1 || !trans1) continue;

        // Dynamic with dynamic
        for (size_t j = i + 1; j < dynamicObjects.size(); ++j)
        {
            EntityID id2 = dynamicObjects[j];
            Collider *col2 = colliders[id2];
            TransformComponent *trans2 = scene->getTransform(id2);

            CollisionInfo result;
            if (testCollision(col1, trans1, id1, col2, trans2, id2, result))
            {
                info.push_back(result);
            }
        }
        
        // Dynamic with MESH
        testMeshCollisions(col1, trans1, id1, info);
    }
}

void PhysicsEngine::testMeshCollisions(Collider *col,
                                       TransformComponent *trans, EntityID id,
                                       std::vector<CollisionInfo> &info)
{
    if (col->getType() != ColliderType::SPHERE &&
        col->getType() != ColliderType::CAPSULE)
        return;

    EntityID id1 = id;
    float minX, maxX, minZ, maxZ;

    SphereCollider *sphere1  = nullptr;
    CapsuleCollider *capsule1 = nullptr;
    glm::vec3 top, bottom;
    float radius;
    float invLengthSq = 0.0f;

    if (col->getType() == ColliderType::SPHERE)
    {
        sphere1 = static_cast<SphereCollider*>(col);
        glm::vec3 spherePos = trans->position + sphere1->offset;
        radius = sphere1->getRadius();
        minX = spherePos.x - radius; maxX = spherePos.x + radius;
        minZ = spherePos.z - radius; maxZ = spherePos.z + radius;
    }
    else
    {
        capsule1 = static_cast<CapsuleCollider*>(col);

        glm::vec3 pos = trans->position + capsule1->offset;

        float halfH = capsule1->getHeight() / 2.0f;

        top    = pos + glm::vec3(0.0f,  halfH, 0.0f);
        bottom = pos - glm::vec3(0.0f,  halfH, 0.0f);

        radius = capsule1->getRadius();

        minX = std::min(top.x, bottom.x) - radius;
        maxX = std::max(top.x, bottom.x) + radius;
        minZ = std::min(top.z, bottom.z) - radius;
        maxZ = std::max(top.z, bottom.z) + radius;

        glm::vec3 capsuleAB = bottom - top;
        invLengthSq = 1.0f / glm::dot(capsuleAB, capsuleAB);
    }

    int minCellX = std::floor(minX / cellSize);
    int maxCellX = std::floor(maxX / cellSize);
    int minCellZ = std::floor(minZ / cellSize);
    int maxCellZ = std::floor(maxZ / cellSize);

    static std::vector<std::pair<EntityID, CollisionInfo>> bestFloor;
    static std::vector<std::pair<EntityID, CollisionInfo>> bestWall;

    bestFloor.clear();
    bestWall.clear();

    for (int x = minCellX; x <= maxCellX; ++x)
    {
        uint64_t hashX = static_cast<uint64_t>(x) << 32;
        for (int z = minCellZ; z <= maxCellZ; ++z)
        {
            uint64_t hash = hashX | (static_cast<uint64_t>(z) & 0xFFFFFFFF);
            auto it = cells.find(hash);
            if (it == cells.end()) continue;

            EntityID lastMeshId = 0;
            MeshCollider *mesh2 = nullptr;
            for (auto &[id2, t] : it->second.trisIndex)
            {
                if (id2 != lastMeshId) 
                {
                    mesh2 = static_cast<MeshCollider*>(colliders[id2]);
                    lastMeshId = id2;
                }
                if (!mesh2) continue;

                auto &triangles = mesh2->getTriangles();

                if (sphere1)
                {
                    glm::vec3 spherePos = trans->position + sphere1->offset;
                    glm::vec3 closest = closestPointOnTriangle(spherePos,
                        triangles[t], triangles[t+1], triangles[t+2]);
                    glm::vec3 diff = spherePos - closest;
                    float distSq = glm::dot(diff, diff);

                    if (distSq < radius * radius)
                    {
                        float dist = std::sqrt(distSq);
                        float depth = radius - dist;
                        
                        CollisionInfo candidate = {
                            id1, id2,
                            dist > 0.0001f ? diff / dist : glm::vec3(0,1,0),
                            depth
                        };

                        bool isFloor = candidate.normal.y > 0.3f;
                        auto &bestList = isFloor ? bestFloor : bestWall;

                        bool found = false;
                        for (auto &[eid, col] : bestList)
                        {
                            if (eid == id2)
                            {
                                if (depth > col.depth) col = candidate;
                                found = true;
                                break;
                            }
                        }
                        if (!found) bestList.push_back({id2, candidate});
                    }
                }
                else // Capsule
                {
                    glm::vec3 closestOnTri = closestPointOnTriangle(bottom,
                                triangles[t], triangles[t+1], triangles[t+2]);
                    
                    glm::vec3 closestOnSeg = closestPointOnSegment(top, bottom,
                                                    closestOnTri, invLengthSq);
                    
                    closestOnTri = closestPointOnTriangle(closestOnSeg,
                                triangles[t], triangles[t+1], triangles[t+2]);

                    glm::vec3 diff = closestOnSeg - closestOnTri;
                    float distSq = glm::dot(diff, diff);

                    if (distSq < radius * radius)
                    {
                        float dist = std::sqrt(distSq);
                        float depth = radius - dist;
                        
                        glm::vec3 edge1 = triangles[t+1] - triangles[t];
                        glm::vec3 edge2 = triangles[t+2] - triangles[t];
                        glm::vec3 triNormal = glm::normalize(glm::cross(edge1, edge2));

                        glm::vec3 normal = dist > 0.0001f ? diff / dist : triNormal;
                        if (glm::dot(normal, triNormal) < 0.0f)
                            normal = -normal;

                        if (glm::dot(normal, triNormal) > 0.85f)
                            normal = triNormal;
                    
                        bool isFloor = normal.y > 0.3f;
                        auto &bestList = isFloor ? bestFloor : bestWall;
                    
                        bool found = false;
                        for (auto &[eid, col] : bestList)
                        {
                            if (eid == id2)
                            {
                                if (depth > col.depth) col = {id1, id2, normal, depth};
                                found = true;
                                break;
                            }
                        }
                        if (!found) bestList.push_back({id2, {id1, id2, normal, depth}});
                    }
                }
            }
        }
    }

    for (auto &[meshId, col] : bestFloor) info.push_back(col);
    for (auto &[meshId, col] : bestWall)  info.push_back(col);
}

void PhysicsEngine::resolveCollision(Scene *scene, CollisionInfo info)
{
    RigidBody *body1 = nullptr;
    RigidBody *body2 = nullptr;

    if (info.entity1 < bodies.size() && bodies[info.entity1] != nullptr) 
    {
        body1 = bodies[info.entity1];
    }

    if (info.entity2 < bodies.size() && bodies[info.entity2] != nullptr) 
    {
        body2 = bodies[info.entity2];
    }

    TransformComponent *trans1 = scene->getTransform(info.entity1);
    TransformComponent *trans2 = scene->getTransform(info.entity2);
    
    if (trans1 == nullptr || trans2 == nullptr) return;

    bool isStatic1 = !body1 || body1->getIsKinematic();
    bool isStatic2 = !body2 || body2->getIsKinematic();

    if (isStatic1 && isStatic2) return;

    float percent = 0.4f;
    float slop = 0.02f;
    float penetration_correction = std::max(info.depth - slop, 0.0f) * percent;

    float totalMass = (isStatic1 ? 0.0f : 1.0f) + (isStatic2 ? 0.0f : 1.0f); 
    float separationPercent = penetration_correction / totalMass; 

    if (!isStatic1) trans1->position -= info.normal * separationPercent;
    if (!isStatic2) trans2->position += info.normal * separationPercent;

    glm::vec3 vel1 = body1 ? body1->getVelocity() : glm::vec3(0.0f);
    glm::vec3 vel2 = body2 ? body2->getVelocity() : glm::vec3(0.0f);

    glm::vec3 relativeVelocity = vel2 - vel1;
    float velAlongNormal = glm::dot(relativeVelocity, info.normal);

    if (velAlongNormal < -0.01f) 
    {
        float restitution = (std::abs(velAlongNormal) < 1.5f) ? 0.0f : 0.2f;

        float impulseScalar = -(1.0f + restitution) * velAlongNormal;
        impulseScalar /= totalMass;
        glm::vec3 impulse = impulseScalar * info.normal;

        if (!isStatic1) body1->setVelocity(body1->getVelocity() - impulse);
        if (!isStatic2) body2->setVelocity(body2->getVelocity() + impulse);
    }

    if (!isStatic1 && info.normal.y > 0.7f) 
    {
        body1->setIsGrounded(true);
        body1->setVelocity(glm::vec3(body1->getVelocity().x, 0.0f, 
                                     body1->getVelocity().z));
    }
    if (!isStatic2 && info.normal.y < -0.7f) 
    {
        body2->setIsGrounded(true);
        body2->setVelocity(glm::vec3(body2->getVelocity().x, 0.0f, 
                                     body2->getVelocity().z));
    }
}

void PhysicsEngine::getCollisionsFor(EntityID id, Scene *scene,
                                     std::vector<CollisionInfo> &outCollisions)
{
    outCollisions.clear();
    auto it = colliders.find(id);
    if (it == colliders.end()) return;
    Collider *mainCollider = it->second;

    for (auto &[otherId, otherCollider] : colliders)
    {
        if (otherId == id) continue;
        if (otherCollider->getType() == ColliderType::MESH) continue;
        CollisionInfo result;
        if (testCollision(
            mainCollider, scene->getTransform(id), id,
            otherCollider, scene->getTransform(otherId), otherId,
            result))
        {
            outCollisions.push_back(result);
        }
    }

    testMeshCollisions(mainCollider, 
                       scene->getTransform(id), 
                       id, 
                       outCollisions);
}

bool PhysicsEngine::testCollision(
    Collider *col1, TransformComponent *trans1, EntityID id1,
    Collider *col2, TransformComponent *trans2, EntityID id2,
    CollisionInfo &result)
{
    ColliderType type1 = col1->getType();
    ColliderType type2 = col2->getType();

    if (type1 > type2)
    {
        std::swap(col1, col2);
        std::swap(trans1, trans2);
        std::swap(type1, type2);
        std::swap(id1, id2);
    }

    SphereCollider *sphere1 = 
        (type1 == ColliderType::SPHERE) ? static_cast<SphereCollider*>(col1) : nullptr;
    BoxCollider *box1 = 
        (type1 == ColliderType::BOX) ? static_cast<BoxCollider*>(col1) : nullptr;
    CapsuleCollider *capsule1 = 
        (type1 == ColliderType::CAPSULE) ? static_cast<CapsuleCollider*>(col1) : nullptr;

    if (type1 == ColliderType::SPHERE && type2 == ColliderType::SPHERE)
    {
        SphereCollider *sphere2 = static_cast<SphereCollider*>(col2);
        glm::vec3 pos1 = trans1->position + sphere1->offset;
        glm::vec3 pos2 = trans2->position + sphere2->offset;
        
        glm::vec3 diff = pos2 - pos1;
        float distSq = glm::dot(diff, diff);
        float sumRadius = sphere1->getRadius() + sphere2->getRadius();
        
        if (distSq <= sumRadius * sumRadius)
        {
            float dist = std::sqrt(distSq);
            result = {id1, id2,
                dist > 0.0001f ? diff / dist : glm::vec3(0,1,0),
                sumRadius - dist};
            return true;
        }
    }
    else if (type1 == ColliderType::CAPSULE && type2 == ColliderType::CAPSULE)
    {
        CapsuleCollider *capsule2 = static_cast<CapsuleCollider*>(col2);
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
        
        glm::vec3 diff = closest1 - closest2;
        float distSq = glm::dot(diff, diff);
        float sumRadius = capsule1->getRadius() + capsule2->getRadius();
        
        if (distSq <= sumRadius * sumRadius)
        {
            float dist = std::sqrt(distSq);
            result = {id1, id2,
                dist > 0.0001f ? diff / dist : glm::vec3(0,1,0),
                sumRadius - dist};
            return true;
        }
    }
    else if (type1 == ColliderType::BOX && type2 == ColliderType::CAPSULE)
    {
        CapsuleCollider *capsule2 = static_cast<CapsuleCollider*>(col2);
        glm::vec3 capPos = trans2->position + capsule2->offset;
        float halfH = capsule2->getHeight() / 2.0f;
        glm::vec3 top = capPos + glm::vec3(0.0f, halfH, 0.0f);
        glm::vec3 bot = capPos - glm::vec3(0.0f, halfH, 0.0f);
        float radius = capsule2->getRadius();
        glm::vec3 boxPos = trans1->position + box1->offset;
        glm::vec3 extents = box1->getHalfExtents();
        glm::vec3 boxMin = boxPos - extents;
        glm::vec3 boxMax = boxPos + extents;

        const int SAMPLES = 16;
        glm::vec3 bestSegPoint = bot;
        float bestDistSq = FLT_MAX;
        for (int i = 0; i <= SAMPLES; ++i)
        {
            float t = (float)i / (float)SAMPLES;
            glm::vec3 p = bot + (top - bot) * t;
            glm::vec3 c = glm::clamp(p, boxMin, boxMax);
            glm::vec3 diff = p - c;
            float dSq = glm::dot(diff, diff);
            if (dSq < bestDistSq)
            {
                bestDistSq = dSq;
                bestSegPoint = p;
            }
        }

        glm::vec3 bestBoxPoint = glm::clamp(bestSegPoint, boxMin, boxMax);
        
        glm::vec3 diff = bestBoxPoint - bestSegPoint;
        float distSq = glm::dot(diff, diff);
        float radiusSq = radius * radius;

        if (distSq <= radiusSq)
        {
            float dist = std::sqrt(distSq);
            glm::vec3 normal;
            float depth;

            if (dist > 0.0001f)
            {
                normal = diff / dist; 
                depth = radius - dist;
            }
            else
            {
                glm::vec3 clampedCenter = glm::clamp(bestSegPoint, boxMin, 
                                                     boxMax);
                float d[6] = {
                    clampedCenter.x - boxMin.x, boxMax.x - clampedCenter.x,
                    clampedCenter.y - boxMin.y, boxMax.y - clampedCenter.y,
                    clampedCenter.z - boxMin.z, boxMax.z - clampedCenter.z
                };
                glm::vec3 normals[6] = {
                    {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1}
                };
                int best = 0;
                for (int k = 1; k < 6; ++k) if (d[k] < d[best]) 
                    best = k;
                
                normal = -normals[best]; 
                depth = radius + d[best];
            }

            result = {id1, id2, normal, depth};
            return true;
        }
    }
    else if (type1 == ColliderType::SPHERE && type2 == ColliderType::CAPSULE)
    {
        CapsuleCollider *capsule2 = static_cast<CapsuleCollider*>(col2);
        
        glm::vec3 capPos = trans2->position + capsule2->offset;
        float halfH = capsule2->getHeight() / 2.0f;
        glm::vec3 top = capPos + glm::vec3(0.0f, halfH, 0.0f);
        glm::vec3 bot = capPos - glm::vec3(0.0f, halfH, 0.0f);
        float capRadius = capsule2->getRadius();

        glm::vec3 spherePos = trans1->position + sphere1->offset;
        float sphereRadius = sphere1->getRadius();

        glm::vec3 capsuleAB = top - bot;
        float invLengthSq = 1.0f / glm::dot(capsuleAB, capsuleAB);

        glm::vec3 closestPointSegment = closestPointOnSegment(bot, top, 
                                                              spherePos,
                                                              invLengthSq);

        glm::vec3 diff = spherePos - closestPointSegment;
        float distSq = glm::dot(diff, diff);
        float sumRadius = sphereRadius + capRadius;

        if (distSq <= sumRadius * sumRadius)
        {
            float dist = std::sqrt(distSq);
            glm::vec3 normal;
            float depth = sumRadius - dist;

            if (dist > 0.0001f)
            {
                normal = diff / dist;
            }
            else
            {
                normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            result = {id1, id2, normal, depth};
            return true;
        }
    }
    return false;
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
    glm::vec3 P, float invLengthSq)
{
    glm::vec3 AB = B - A;
    float t = glm::dot(P - A, AB) * invLengthSq;
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
