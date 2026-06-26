#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

struct PositionKey { float time; glm::vec3 value; };
struct RotationKey { float time; glm::quat value; };
struct ScaleKey { float time; glm::vec3 value; };

struct AnimationChannel
{
    int boneIndex;
    std::vector<PositionKey> positions;
    std::vector<RotationKey> rotations;
    std::vector<ScaleKey> scales;
};

struct Animation
{
    std::string name;
    float duration;
    std::vector<AnimationChannel> channels;
};

struct Bone
{
    std::string name;
    int parentIndex; // -1 root
    glm::mat4 inverseBindMatrix;
    glm::mat4 localTransform;
};

struct Skeleton
{
    std::vector<Bone> bones;
    std::vector<Animation> animations;
};