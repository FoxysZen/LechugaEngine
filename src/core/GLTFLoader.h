#pragma once
#include <Animation.h>
#include <Texture.h>
#include <string>
#include <vector>

struct SkinnedVertex
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
    int boneIDs[4];
    float boneWeights[4];
};

struct SkinnedSubMesh
{
    std::vector<SkinnedVertex> vertices;
    std::vector<unsigned int> indices;
    std::string texturePath;
    std::vector<unsigned char> embeddedTexture;
    std::string embeddedMimeType;
};

struct SkinnedMeshData
{
    std::vector<SkinnedSubMesh> subMeshes;
    Skeleton skeleton;
};

class GLTFLoader
{
    public:
        static SkinnedMeshData load(const std::string &path, 
                    const std::string &texturesBasePath = "assets/textures/");
};