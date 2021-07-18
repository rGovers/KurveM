#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec4 vBones;
layout(location = 4) in vec4 vWeights;

layout(location = 5) uniform uint BoneCount;
layout(location = 6) uniform uint Bone;

layout(location = 0) out vec4 color;

void main()
{
    float val = 0;

    val += float((vBones.x * BoneCount) == Bone) * vWeights.x;
    val += float((vBones.y * BoneCount) == Bone) * vWeights.y;
    val += float((vBones.z * BoneCount) == Bone) * vWeights.z;
    val += float((vBones.w * BoneCount) == Bone) * vWeights.w;

    color = vec4(val.xxx, 1.0f);
}