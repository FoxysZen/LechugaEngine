#pragma once
#include <Mesh.h>
#include <ShaderProgram.h>
#include <Texture.h>

struct LOD {
    Mesh* mesh;
    float maxDistance;
};

struct MeshComponent {
    std::vector<LOD> lods;
    ShaderProgram *shader;
    std::vector<Texture*> textures;
};