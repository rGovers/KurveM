#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) uniform mat4 View;
layout(location = 1) uniform mat4 Projection;
layout(location = 2) uniform mat4 World;

layout(location = 0) out vec2 vUV;

void main()
{
    vec2 pos = vec2(gl_VertexID % 2, gl_VertexID / 2);

    vUV = pos;

    gl_Position = Projection * View * World * vec4((pos.xy * 2 - 1) * 0.5, 0, 1);
}