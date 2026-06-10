#pragma once
#include <glad/glad.h>
#include <Logger.h>
#include <Texture.h>
#include <vector>

struct SubMesh {
    GLuint VAO, VBO;
    int vertexCount;
    Texture* texture;
};

class Mesh
{
    public:
        Mesh();
        ~Mesh();

        void addSubMesh(std::vector<float> vertices, Texture* texture);
        void draw();
    
    private:
        std::vector<SubMesh> subMeshes;
};