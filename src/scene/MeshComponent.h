#pragma once
#include <Mesh.h>
#include <ShaderProgram.h>
#include <Texture.h>

struct MeshComponent {
    Mesh *mesh;
    ShaderProgram *shader;
    std::vector<Texture*> textures;
};