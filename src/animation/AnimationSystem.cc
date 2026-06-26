#include <AnimationSystem.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

void AnimationSystem::setMesh(SkinnedMesh *m)
{
    mesh = m;
    boneMatrices.assign(mesh->getSkeleton().bones.size(), glm::mat4(1.0f));
}

void AnimationSystem::play(const std::string &name, bool loopAnim)
{
    if (!mesh) return;
    const auto &anims = mesh->getSkeleton().animations;
    for (int i = 0; i < (int)anims.size(); ++i)
    {
        if (anims[i].name == name)
        {
            animIndex = i;
            currentTime = 0.0f;
            playing = true;
            loop = loopAnim;
            currentAnim = name;
            return;
        }
    }
}

void AnimationSystem::stop()
{
    playing = false;
    currentTime = 0.0f;
}

void AnimationSystem::update(float deltaTime)
{
    if (!mesh || !playing || animIndex < 0) return;

    const Animation &anim = mesh->getSkeleton().animations[animIndex];
    currentTime += deltaTime;

    if (currentTime > anim.duration)
    {
        if (loop) 
            currentTime = fmodf(currentTime, anim.duration);
        else
        {
            currentTime = anim.duration;
            playing = false;
        }
    }

    computeBoneMatrices(anim);
}

void AnimationSystem::computeBoneMatrices(const Animation& anim)
{
    const auto &bones = mesh->getSkeleton().bones;
    int boneCount = (int)bones.size();

    std::vector<glm::mat4> globalTransforms(boneCount, glm::mat4(1.0f));

    for (int i = 0; i < boneCount; ++i)
    {
        // Repose pose by default
        glm::mat4 localTransform = bones[i].localTransform;

        for (const auto &ch : anim.channels)
        {
            if (ch.boneIndex != i) continue;

            glm::vec3 pos = interpolatePosition(ch, currentTime);
            glm::quat rot = interpolateRotation(ch, currentTime);
            glm::vec3 scale = interpolateScale(ch, currentTime);

            localTransform = glm::translate(glm::mat4(1.0f), pos)
                           * glm::mat4_cast(rot)
                           * glm::scale(glm::mat4(1.0f), scale);
            break;
        }

        if (bones[i].parentIndex == -1)
            globalTransforms[i] = localTransform;
        else
            globalTransforms[i] = globalTransforms[bones[i].parentIndex] * localTransform;
    }

    boneMatrices.resize(boneCount);
    for (int i = 0; i < boneCount; ++i)
        boneMatrices[i] = globalTransforms[i] * bones[i].inverseBindMatrix;
}

// Interpolation
glm::vec3 AnimationSystem::interpolatePosition(const AnimationChannel &ch, 
                                               float time)
{
    if (ch.positions.empty()) return glm::vec3(0.0f);
    if (ch.positions.size() == 1) return ch.positions[0].value;

    for (int i = 0; i < (int)ch.positions.size() - 1; ++i)
    {
        if (time <= ch.positions[i + 1].time)
        {
            float t0 = ch.positions[i].time;
            float t1 = ch.positions[i + 1].time;
            float alpha = (time - t0) / (t1 - t0);
            return glm::mix(ch.positions[i].value, ch.positions[i + 1].value, alpha);
        }
    }
    return ch.positions.back().value;
}

glm::quat AnimationSystem::interpolateRotation(const AnimationChannel &ch, float time)
{
    if (ch.rotations.empty())
        return glm::quat(1, 0, 0, 0);
    if (ch.rotations.size() == 1)
        return ch.rotations[0].value;

    for (int i = 0; i < (int)ch.rotations.size() - 1; ++i)
    {
        if (time <= ch.rotations[i + 1].time)
        {
            float t0 = ch.rotations[i].time;
            float t1 = ch.rotations[i + 1].time;
            float alpha = (time - t0) / (t1 - t0);
            return glm::slerp(ch.rotations[i].value, 
                              ch.rotations[i + 1].value,
                              alpha);
        }
    }
    return ch.rotations.back().value;
}

glm::vec3 AnimationSystem::interpolateScale(const AnimationChannel &ch, float time)
{
    if (ch.scales.empty())
        return glm::vec3(1.0f);
    if (ch.scales.size() == 1)
        return ch.scales[0].value;

    for (int i = 0; i < (int)ch.scales.size() - 1; ++i)
    {
        if (time <= ch.scales[i + 1].time)
        {
            float t0 = ch.scales[i].time;
            float t1 = ch.scales[i + 1].time;
            float alpha = (time - t0) / (t1 - t0);
            return glm::mix(ch.scales[i].value, ch.scales[i + 1].value, alpha);
        }
    }
    return ch.scales.back().value;
}

const std::vector<glm::mat4> &AnimationSystem::getBoneMatrices() const
{
    return boneMatrices;
}