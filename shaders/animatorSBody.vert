#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 5) in vec4 BodyI;
layout(location = 6) in vec4 BodyW;

layout(location = 0) uniform mat4 View;
layout(location = 1) uniform mat4 Projection;
layout(location = 2) uniform uint NodeCount;

layout(std140, binding = 3) buffer BodyData
{
    vec4 Delta[];
};

layout(location = 0) out vec4 vPosition;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec2 vTexCoord;

void main()
{
    vec4 d = Delta[int(BodyI.x * NodeCount)] * BodyW.x + 
        Delta[int(BodyI.y * NodeCount)] * BodyW.y + 
        Delta[int(BodyI.z * NodeCount)] * BodyW.z +
        Delta[int(BodyI.w * NodeCount)] * BodyW.w;

    vec3 dN = normalize(d.xyz);

    vPosition = gl_Position = Projection * View * (Position + d);
    vNormal = normalize(Normal - dN);
    vTexCoord = TexCoord;
}
