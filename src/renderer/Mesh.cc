#include <Mesh.h>

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
    for (auto& sub : subMeshes)
    {
        glDeleteVertexArrays(1, &sub.VAO);
        glDeleteBuffers(1, &sub.VBO);
    }
}

void Mesh::addSubMesh(std::vector<float> vertices, Texture* texture)
{
    for (int i = 0; i < vertices.size(); i += 8)
    {
        glm::vec3 pos(vertices[i], vertices[i+1], vertices[i+2]);
        allPositions.push_back(pos);
    }

    SubMesh sub;
    sub.vertexCount = vertices.size() / 8;
    sub.texture = texture;
    
    glGenVertexArrays(1, &sub.VAO);
    glGenBuffers(1, &sub.VBO);
    glBindVertexArray(sub.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, sub.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
        vertices.data(), GL_STATIC_DRAW);
    // Vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    subMeshes.push_back(sub);
}

void Mesh::calculateBounds()
{
    glm::vec3 sum(0.0f);
    for (auto& pos : allPositions)
    {
        sum += pos;
    }
    boundsCenter = sum / (float)allPositions.size();
    
    boundsRadius = 0.0f;
    for (auto& pos : allPositions)
    {
        float dist = glm::length(pos - boundsCenter);
        if (dist > boundsRadius)
        {
            boundsRadius = dist;
        }
    }
    
    allPositions.clear();
}

float Mesh::getBoundsRadius()
{
    return boundsRadius;
}

glm::vec3 Mesh::getBoundsCenter()
{
    return boundsCenter;
}

void Mesh::draw()
{
    for (auto& sub : subMeshes)
    {
        if (sub.texture)
        {
            sub.texture->bind(0);
        }
        glBindVertexArray(sub.VAO);
        glDrawArrays(GL_TRIANGLES, 0, sub.vertexCount);
        if (sub.texture)
        {
            sub.texture->unbind(0);
        }
    }
}
