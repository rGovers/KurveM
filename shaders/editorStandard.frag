#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

layout(location = 0) uniform mat4 View;
layout(location = 10) uniform sampler2D MainTex;

layout(location = 0) out vec4 color;

const float shine = 10.0f;

const mat3 lightMat = mat3(
   0.5602514,  0.4397486,  0.7019541,
   0.4397486,  0.5602514, -0.7019541,
  -0.7019541,  0.7019541,  0.1205028);

void main()
{
    // Rotating light with the camera
    // Using to ensure lighting in editor on all angles
    mat3 lfMat = mat3(View) * lightMat;
    vec3 lDir = lfMat[2];
    float l = max(dot(lDir, vNormal), 0.0f);

    float lS = max(sign(l), 0.0f);

    vec3 vDir = normalize(-vPosition.xyz);

    vec3 hDir = normalize(lDir + vDir);
    float sA = max(dot(hDir, vNormal), 0.0f);
    float s = lS * pow(sA, shine);

    color = vec4(0.5f.xxx * l + s.xxx, 1.0f);
}