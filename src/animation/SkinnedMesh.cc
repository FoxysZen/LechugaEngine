#include <SkinnedMesh.h>

SkinnedMesh::SkinnedMesh() {}

SkinnedMesh::~SkinnedMesh()
{
    for (auto &sub : subMeshes)
    {
        glDeleteVertexArrays(1, &sub.VAO);
        glDeleteBuffers(1, &sub.VBO);
        glDeleteBuffers(1, &sub.EBO);
    }
}

void SkinnedMesh::addSubMesh(const std::vector<SkinnedVertex> &vertices,
                             const std::vector<unsigned int> &indices,
                             Texture *texture)
{
    for (const auto &v : vertices)
    {
        allPositions.push_back(glm::vec3(v.x, v.y, v.z));
    }

    SkinnedSubMeshGPU sub;
    sub.texture = texture;
    sub.indexCount = (int)indices.size();

    glGenVertexArrays(1, &sub.VAO);
    glGenBuffers(1, &sub.VBO);
    glGenBuffers(1, &sub.EBO);

    glBindVertexArray(sub.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, sub.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(SkinnedVertex),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sub.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    int stride = sizeof(SkinnedVertex);

    // location 0: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(SkinnedVertex, x));
    glEnableVertexAttribArray(0);

    // location 1: uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(SkinnedVertex, u));
    glEnableVertexAttribArray(1);

    // location 2: normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(SkinnedVertex, nx));
    glEnableVertexAttribArray(2);

    // location 3: bone IDs (int)
    glVertexAttribIPointer(3, 4, GL_INT, stride,
                           (void*)offsetof(SkinnedVertex, boneIDs));
    glEnableVertexAttribArray(3);

    // location 4: bone weights
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(SkinnedVertex, boneWeights));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
    subMeshes.push_back(sub);
}

void SkinnedMesh::setSkeleton(const Skeleton &s)
{
    skeleton = s;
}

Skeleton &SkinnedMesh::getSkeleton()
{
    return skeleton;
}

void SkinnedMesh::draw()
{
    for (auto &sub : subMeshes)
    {
        if (sub.texture) sub.texture->bind(0);
        glBindVertexArray(sub.VAO);
        glDrawElements(GL_TRIANGLES, sub.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        if (sub.texture) sub.texture->unbind(0);
    }

    //for (auto &sub : subMeshes)
    //{
    //    if (sub.texture) sub.texture->bind(0);
    //    glBindVertexArray(sub.VAO);
    //    
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
    //    glDrawElements(GL_TRIANGLES, sub.indexCount, GL_UNSIGNED_INT, 0);
    //    
    //    if (sub.texture) sub.texture->unbind(0);
//
    //    glEnable(GL_POLYGON_OFFSET_LINE);
    //    glPolygonOffset(-1.0f, -1.0f);
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //    
    //    glDrawElements(GL_TRIANGLES, sub.indexCount, GL_UNSIGNED_INT, 0);
    //    
    //    glDisable(GL_POLYGON_OFFSET_LINE);
    //    glBindVertexArray(0);
    //}
    //
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void SkinnedMesh::calculateBounds()
{
    if (allPositions.empty()) return;

    glm::vec3 sum(0.0f);
    for (const auto &pos : allPositions)
    {
        sum += pos;
    }
    boundsCenter = sum / (float)allPositions.size();
    
    boundsRadius = 0.0f;
    for (const auto &pos : allPositions)
    {
        float dist = glm::length(pos - boundsCenter);
        if (dist > boundsRadius)
        {
            boundsRadius = dist;
        }
    }
    
    // Adds a 35% bound to prevent disappearing in some animations
    boundsRadius *= 1.35f;
    allPositions.clear();
}

float SkinnedMesh::getBoundsRadius()
{
    return boundsRadius;
}

glm::vec3 SkinnedMesh::getBoundsCenter()
{
    return boundsCenter;
}
