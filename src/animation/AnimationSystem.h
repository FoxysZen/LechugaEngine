#pragma once
#include "Animation.h"
#include "SkinnedMesh.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

class AnimationSystem
{
public:
    AnimationSystem() = default;

    void setMesh(SkinnedMesh *mesh);

    void play(const std::string &name, bool loop = true);
    void stop();

    void update(float deltaTime);

    const std::vector<glm::mat4> &getBoneMatrices() const;

    bool isPlaying() const { return playing; }
    const std::string &currentAnimation() const { return currentAnim; }

private:
    SkinnedMesh *mesh = nullptr;
    int animIndex = -1;
    float currentTime = 0.0f;
    bool playing = false;
    bool loop = true;
    std::string currentAnim;

    std::vector<glm::mat4> boneMatrices;

    void computeBoneMatrices(const Animation &anim);

    glm::vec3 interpolatePosition(const AnimationChannel &ch, float time);
    glm::quat interpolateRotation(const AnimationChannel &ch, float time);
    glm::vec3 interpolateScale(const AnimationChannel &ch, float time);
};