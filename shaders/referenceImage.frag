#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 vUV;

layout(location = 4) uniform sampler2D MainTex;

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(texture(MainTex, vUV).xyz, 1.0f);
}