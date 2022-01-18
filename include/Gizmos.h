#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <vector>

struct BezierCurveNode3;
struct Vertex;

class Camera;
class LocalModel;
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
    static void DrawAll();
    static void DrawAll(const Camera* a_camera, const glm::vec2& a_winSize); 

    static void DrawModel(const glm::mat4& a_transform, const Vertex* a_vertices, unsigned int a_vertexCount, const unsigned int* a_indices, unsigned int a_indexCount, const glm::vec4& a_color);
    static void DrawModel(const glm::mat4& a_transform, const LocalModel* a_model, const glm::vec4& a_color);

    static void DrawLine(const glm::vec3& a_start, const glm::vec3& a_end, float a_width, const glm::vec4& a_color);
    static void DrawLine(const glm::vec3& a_start, const glm::vec3& a_end, const glm::vec3& a_up, float a_width, const glm::vec4& a_color);

    static void DrawCurve(int a_steps, const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB, const glm::vec4& a_color);
    static void DrawCurve(int a_steps, const glm::mat4& a_transform, const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB, const glm::vec4& a_color);

    static void DrawCircle(const glm::vec3& a_position, const glm::vec3& a_dir, float a_radius, float a_width, int a_steps, const glm::vec4& a_color);
    static void DrawCircleFilled(const glm::vec3& a_position, const glm::vec3& a_dir, float a_radius, int a_steps, const glm::vec4& a_color);

    static void DrawTriangle(const glm::vec3& a_position, const glm::vec3& a_dir, float a_size, const glm::vec4& a_color);
    static void DrawTriangle(const glm::vec3& a_position, const glm::vec3& a_dir, const glm::vec3& a_up, float a_size, const glm::vec4& a_color);

    static void DrawTranslation(const glm::vec3& a_position, const glm::vec3& a_dir, float a_scale);
    static void DrawScale(const glm::vec3& a_position, const glm::vec3& a_dir, float a_scale);
    static void DrawRotation(const glm::vec3& a_position, const glm::vec3& a_dir, float a_scale);
};