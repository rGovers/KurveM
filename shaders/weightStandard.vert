#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec4 bones;
layout(location = 4) in vec4 weights;

layout(location = 0) uniform mat4 View;
layout(location = 1) uniform mat4 Projection;
layout(location = 2) uniform mat4 World;

layout(location = 0) out vec4 vPosition;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec2 vTexCoord;
layout(location = 3) out vec4 vBones;
layout(location = 4) out vec4 vWeights;

void main()
{
    vNormal = mat3(World) * normal;
    vTexCoord = texCoord;

    vBones = bones;
    vWeights = weights;

    vPosition = gl_Position = Projection * View * (World * position);
}
