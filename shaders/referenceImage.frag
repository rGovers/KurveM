#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 vUV;

layout(location = 4) uniform sampler2D MainTex;

layout(location = 0) out vec4 color;

void main()
{
    vec4 c = texture(MainTex, vUV);

    if (c.a <= 0.5)
    {
        discard;
    }

    color = vec4(c.xyz, 1.0f);
}