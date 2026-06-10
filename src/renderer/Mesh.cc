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
    SubMesh sub;
    sub.vertexCount = vertices.size() / 5;
    sub.texture = texture;
    
    glGenVertexArrays(1, &sub.VAO);
    glGenBuffers(1, &sub.VBO);
    glBindVertexArray(sub.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, sub.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
        vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    subMeshes.push_back(sub);
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
