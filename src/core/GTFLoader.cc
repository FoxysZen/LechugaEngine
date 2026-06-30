#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <GLTFLoader.h>
#include <Logger.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

template<typename T>
static std::vector<T> readAccessor(const cgltf_accessor *accessor)
{
    std::vector<T> result(accessor->count);
    for (size_t i = 0; i < accessor->count; ++i)
        cgltf_accessor_read_float(accessor, i, (float*)&result[i],
                                   sizeof(T) / sizeof(float));
    return result;
}

static glm::mat4 nodeLocalTransform(const cgltf_node *node)
{
    if (node->has_matrix)
        return glm::make_mat4(node->matrix);

    glm::mat4 T(1.0f), R(1.0f), S(1.0f);
    if (node->has_translation)
    {
        T = glm::translate(glm::mat4(1.0f),
                glm::vec3(node->translation[0], node->translation[1], 
                          node->translation[2]));
    }
    if (node->has_rotation)
    {
        R = glm::mat4_cast(glm::quat(node->rotation[3], node->rotation[0],
                                     node->rotation[1], node->rotation[2]));
    }
    if (node->has_scale)
    {
        S = glm::scale(glm::mat4(1.0f),
                glm::vec3(node->scale[0], node->scale[1], node->scale[2]));
    }
    return T * R * S;
}

SkinnedMeshData GLTFLoader::load(const std::string &path,
                                 const std::string &texturesBasePath)
{
    SkinnedMeshData result;

    cgltf_options options = {};
    cgltf_data *data = nullptr;

    if (cgltf_parse_file(&options, path.c_str(), &data) != cgltf_result_success)
    {
        Logger::error("GLTFLoader: failed to parse " + path);
        return result;
    }
    if (cgltf_load_buffers(&options, data, path.c_str()) != cgltf_result_success)
    {
        Logger::error("GLTFLoader: failed to load buffers " + path);
        cgltf_free(data);
        return result;
    }

    // Skelleton
    if (data->skins_count > 0)
    {
        const cgltf_skin &skin = data->skins[0];
        result.skeleton.bones.resize(skin.joints_count);

        std::vector<glm::mat4> invBinds(skin.joints_count, glm::mat4(1.0f));
        if (skin.inverse_bind_matrices)
        {
            for (size_t i = 0; i < skin.joints_count; ++i)
                cgltf_accessor_read_float(skin.inverse_bind_matrices, i,
                                          glm::value_ptr(invBinds[i]), 16);
        }

        for (size_t i = 0; i < skin.joints_count; ++i)
        {
            const cgltf_node* joint = skin.joints[i];
            Bone& bone = result.skeleton.bones[i];
            bone.name = joint->name ? joint->name : "bone_" + std::to_string(i);
            bone.inverseBindMatrix = invBinds[i];
            bone.localTransform = nodeLocalTransform(joint);

            bone.parentIndex = -1;
            if (joint->parent)
            {
                for (size_t j = 0; j < skin.joints_count; ++j)
                {
                    if (skin.joints[j] == joint->parent)
                    {
                        bone.parentIndex = (int)j;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        result.skeleton.bones.resize(1);
        Bone &rootBone = result.skeleton.bones[0];
        rootBone.name = "Static_Root";
        rootBone.inverseBindMatrix = glm::mat4(1.0f);
        rootBone.localTransform = glm::mat4(1.0f);
        rootBone.parentIndex = -1;
    }

    // Geometry
    for (size_t mi = 0; mi < data->meshes_count; ++mi)
    {
        const cgltf_mesh &mesh = data->meshes[mi];
        for (size_t pi = 0; pi < mesh.primitives_count; ++pi)
        {
            const cgltf_primitive &prim = mesh.primitives[pi];
            SkinnedSubMesh sub;

            std::vector<glm::vec3> positions, normals;
            std::vector<glm::vec2> uvs;
            std::vector<glm::vec4> weights;
            std::vector<glm::uvec4> joints;

            for (size_t ai = 0; ai < prim.attributes_count; ++ai)
            {
                const cgltf_attribute &attr = prim.attributes[ai];
                if (attr.type == cgltf_attribute_type_position)
                    positions = readAccessor<glm::vec3>(attr.data);
                else if (attr.type == cgltf_attribute_type_normal)
                    normals = readAccessor<glm::vec3>(attr.data);
                else if (attr.type == cgltf_attribute_type_texcoord && 
                         attr.index == 0)
                    uvs = readAccessor<glm::vec2>(attr.data);
                else if (attr.type == cgltf_attribute_type_weights && 
                         attr.index == 0)
                    weights = readAccessor<glm::vec4>(attr.data);
                else if (attr.type == cgltf_attribute_type_joints && attr.index == 0)
                {
                    joints.resize(attr.data->count);
                    for (size_t i = 0; i < attr.data->count; ++i)
                    {
                        float tmp[4];
                        cgltf_accessor_read_float(attr.data, i, tmp, 4);
                        joints[i] = glm::uvec4(
                            (unsigned int)tmp[0],
                            (unsigned int)tmp[1],
                            (unsigned int)tmp[2],
                            (unsigned int)tmp[3]
                        );
                    }
                }
            }

            size_t vertCount = positions.size();
            sub.vertices.resize(vertCount);
            for (size_t vi = 0; vi < vertCount; ++vi)
            {
                SkinnedVertex &sv = sub.vertices[vi];
                sv.x = positions[vi].x;
                sv.y = positions[vi].y;
                sv.z = positions[vi].z;
                sv.nx = normals.size() > vi ? normals[vi].x : 0.0f;
                sv.ny = normals.size() > vi ? normals[vi].y : 1.0f;
                sv.nz = normals.size() > vi ? normals[vi].z : 0.0f;
                sv.u = uvs.size() > vi ? uvs[vi].x : 0.0f;
                sv.v = uvs.size() > vi ? 1.0f - uvs[vi].y : 0.0f;

                if (joints.size() > vi)
                {
                    sv.boneIDs[0] = joints[vi].x; sv.boneIDs[1] = joints[vi].y;
                    sv.boneIDs[2] = joints[vi].z; sv.boneIDs[3] = joints[vi].w;
                }
                else
                {
                    sv.boneIDs[0] = sv.boneIDs[1] = sv.boneIDs[2] = sv.boneIDs[3] = 0;
                }

                if (weights.size() > vi)
                {
                    sv.boneWeights[0] = weights[vi].x;
                    sv.boneWeights[1] = weights[vi].y;
                    sv.boneWeights[2] = weights[vi].z;
                    sv.boneWeights[3] = weights[vi].w;
                }
                else
                {
                    sv.boneWeights[0] = 1.0f;
                    sv.boneWeights[1] = sv.boneWeights[2] = sv.boneWeights[3] = 0.0f;
                }
            }

            if (prim.indices)
            {
                sub.indices.resize(prim.indices->count);
                for (size_t ii = 0; ii < prim.indices->count; ++ii)
                    sub.indices[ii] = 
                        (unsigned int)cgltf_accessor_read_index(prim.indices, 
                                                                ii);
            }

            // Texture
            if (prim.material && prim.material->pbr_metallic_roughness.base_color_texture.texture)
            {
                const cgltf_image* image = prim.material->pbr_metallic_roughness
                                               .base_color_texture.texture->image;
                if (image->uri)
                {
                    // Textura externa (gltf separado)
                    sub.texturePath = texturesBasePath + image->uri;
                }
                else if (image->buffer_view)
                {
                    // Textura embebida en el glb — extraer los bytes crudos
                    const cgltf_buffer_view* bv = image->buffer_view;
                    const unsigned char* bufferData = (const unsigned char*)bv->buffer->data + bv->offset;
                    sub.embeddedTexture.assign(bufferData, bufferData + bv->size);
                    sub.embeddedMimeType = image->mime_type ? image->mime_type : "image/png";
                }
            }

            result.subMeshes.push_back(std::move(sub));
        }
    }

    // Animations
    for (size_t ai = 0; ai < data->animations_count; ++ai)
    {
        const cgltf_animation &anim = data->animations[ai];
        Animation animation;
        animation.name = anim.name ? anim.name : "anim_" + std::to_string(ai);
        animation.duration = 0.0f;

        for (size_t ci = 0; ci < anim.channels_count; ++ci)
        {
            const cgltf_animation_channel &ch = anim.channels[ci];
            if (!ch.target_node) continue;

            int boneIdx = -1;
            if (data->skins_count > 0)
            {
                const cgltf_skin &skin = data->skins[0];
                for (size_t ji = 0; ji < skin.joints_count; ++ji)
                {
                    if (skin.joints[ji] == ch.target_node)
                    {
                        boneIdx = (int)ji;
                        break;
                    }
                }
            }
            if (boneIdx == -1) continue;

            AnimationChannel *channel = nullptr;
            for (auto &c : animation.channels)
            {
                if (c.boneIndex == boneIdx)
                {
                    channel = &c; break;
                }
            }
            if (!channel)
            {
                animation.channels.push_back({boneIdx, {}, {}, {}});
                channel = &animation.channels.back();
            }

            std::vector<float> times(ch.sampler->input->count);
            for (size_t ti = 0; ti < ch.sampler->input->count; ++ti)
            {
                cgltf_accessor_read_float(ch.sampler->input, ti, &times[ti], 1);
                animation.duration = std::max(animation.duration, times[ti]);
            }

            if (ch.target_path == cgltf_animation_path_type_translation)
            {
                for (size_t ki = 0; ki < times.size(); ++ki)
                {
                    glm::vec3 val;
                    cgltf_accessor_read_float(ch.sampler->output, ki,
                                              glm::value_ptr(val), 3);
                    channel->positions.push_back({times[ki], val});
                }
            }
            else if (ch.target_path == cgltf_animation_path_type_rotation)
            {
                for (size_t ki = 0; ki < times.size(); ++ki)
                {
                    float q[4];
                    cgltf_accessor_read_float(ch.sampler->output, ki, q, 4);
                    // glTF: x,y,z,w — glm::quat: w,x,y,z
                    channel->rotations.push_back({times[ki], glm::quat(q[3], 
                                                                       q[0], 
                                                                       q[1], 
                                                                       q[2])});
                }
            }
            else if (ch.target_path == cgltf_animation_path_type_scale)
            {
                for (size_t ki = 0; ki < times.size(); ++ki)
                {
                    glm::vec3 val;
                    cgltf_accessor_read_float(ch.sampler->output, ki,
                                              glm::value_ptr(val), 3);
                    channel->scales.push_back({times[ki], val});
                }
            }
        }
        result.skeleton.animations.push_back(std::move(animation));
    }

    cgltf_free(data);
    return result;
}