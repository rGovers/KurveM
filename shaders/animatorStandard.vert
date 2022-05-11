#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec4 Bones;
layout(location = 4) in vec4 Weights;

layout(location = 0) uniform mat4 View;
layout(location = 1) uniform mat4 Projection;
layout(location = 2) uniform mat4 World;
layout(location = 3) uniform uint BoneCount;

layout(std140, binding = 4) buffer BoneData
{
    mat4 BoneMatrices[];
};

layout(location = 0) out vec4 vPosition;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec2 vTexCoord;

void main()
{
    float s = 1 - max(sign(dot(vec4(1), Weights)), 0.0f);

    mat4 mat = mat4(s);
    mat += BoneMatrices[uint(Bones.x * BoneCount)] * Weights.x;
    mat += BoneMatrices[uint(Bones.y * BoneCount)] * Weights.y;
    mat += BoneMatrices[uint(Bones.z * BoneCount)] * Weights.z;
    mat += BoneMatrices[uint(Bones.w * BoneCount)] * Weights.w;

    vPosition = gl_Position = Projection * View * (World * (mat * Position));

    vNormal = normalize(mat3(World) * (mat3(mat) * Normal));
    vTexCoord = TexCoord;
}
