#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
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
        void calculateBounds();
        float getBoundsRadius();
        glm::vec3 getBoundsCenter();
        void draw();
    
    private:
        std::vector<SubMesh> subMeshes;
        std::vector<glm::vec3> allPositions;
        glm::vec3 boundsCenter = glm::vec3(0.0f);
        float boundsRadius = 0.0f;
};