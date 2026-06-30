#pragma once
#include <Animation.h>
#include <Texture.h>
#include <GLTFLoader.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct SkinnedSubMeshGPU
{
    GLuint   VAO, VBO, EBO;
    int      indexCount;
    Texture *texture = nullptr;
};

class SkinnedMesh
{
    public:
        SkinnedMesh();
        ~SkinnedMesh();

        void addSubMesh(const std::vector<SkinnedVertex> &vertices,
                        const std::vector<unsigned int> &indices,
                        Texture *texture);
        void setSkeleton(const Skeleton &skeleton);
        Skeleton &getSkeleton();

        void calculateBounds();
        float getBoundsRadius();
        glm::vec3 getBoundsCenter();

        void draw();

    private:
        std::vector<SkinnedSubMeshGPU> subMeshes;
        Skeleton skeleton;

        std::vector<glm::vec3> allPositions;
        glm::vec3 boundsCenter = glm::vec3(0.0f);
        float boundsRadius = 0.0f;
};