#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

layout(location = 0) uniform mat4 View;

layout(location = 0) out vec4 color;

const float shine = 5.0f;

const vec3 lightDir = vec3(0.1f, -0.1f, -1.0f);

void main()
{
    // Rotating light with the camera
    // Using to ensure lighting in editor on all angles
    vec3 lDir = inverse(mat3(View)) * normalize(lightDir);
    float l = max(dot(lDir, vNormal), 0.0f);

    float lS = max(sign(l), 0.0f);

    vec3 vDir = normalize(-vPosition.xyz);

    vec3 hDir = normalize(lDir + vDir);
    float sA = max(dot(hDir, vNormal), 0.0f);
    float s = lS * pow(sA, shine);

    color = vec4(0.5f.xxx * l.xxx + s.xxx, 1.0f);
}