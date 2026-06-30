#pragma once
#include <AnimationSystem.h>
#include <ShaderProgram.h>
#include <SkinnedMesh.h>

struct SkinnedLOD {
    SkinnedMesh *mesh;
    float maxDistance;
};

struct SkinnedMeshComponent
{
    std::vector<SkinnedLOD> lods;
    ShaderProgram *shader = nullptr;
    AnimationSystem *animSys = nullptr;
};