#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <vector>

class Camera;
class ShaderProgram;

struct GizmoVertex
{
    glm::vec4 Position;
    glm::vec4 Color;
};

class Gizmos
{
private:
    static Gizmos* Instance;

    unsigned int              m_vbo;
    unsigned int              m_ibo;
    unsigned int              m_vao;

    ShaderProgram*            m_program;

    std::vector<GizmoVertex>  m_vertices;
    std::vector<unsigned int> m_indices;

    Gizmos();
protected:

public:
    ~Gizmos();

    static void Init();
    static void Destroy();

    static void Clear();
    static void DrawAll(Camera* a_camera, const glm::vec2& a_winSize); 

    static void DrawLine(const glm::vec3& a_start, const glm::vec3& a_end, float a_width, const glm::vec4& a_color);
    static void DrawLine(const glm::vec3& a_start, const glm::vec3& a_end, const glm::vec3& a_up, float a_width, const glm::vec4& a_color);

    static void DrawCircle(const glm::vec3& a_position, const glm::vec3& a_dir, float a_radius, int a_steps, const glm::vec4& a_color);
    static void DrawCircleFilled(const glm::vec3& a_position, const glm::vec3& a_dir, float a_radius, int a_steps, const glm::vec4& a_color);

    static void DrawTriangle(const glm::vec3& a_position, const glm::vec3& a_dir, float a_size, const glm::vec4& a_color);
};