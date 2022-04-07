#include "Gizmos.h"

#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "BezierCurveNode3.h"
#include "Camera.h"
#include "Datastore.h"
#include "LocalModel.h"
#include "ShaderPixel.h"
#include "ShaderProgram.h"
#include "Shaders/GizmosPixel.h"
#include "Shaders/GizmosVertex.h"
#include "ShaderVertex.h"
#include "TransformVisualizer.h"

Gizmos* Gizmos::Instance = nullptr;

Gizmos::Gizmos()
{
    m_program = Datastore::GetShaderProgram("SHADER_GIZMOS");
    if (m_program == nullptr)
    {
        ShaderVertex* vertexShader = new ShaderVertex(GIZMOSVERTEX);
        ShaderPixel* pixelShader = new ShaderPixel(GIZMOSPIXEL);

        m_program = new ShaderProgram(vertexShader, pixelShader);

        delete vertexShader;
        delete pixelShader;

        Datastore::AddShaderProgram("SHADER_GIZMOS", m_program);
    }

    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4,  GL_FLOAT, false, sizeof(GizmoVertex), (void*)offsetof(GizmoVertex, Position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4,  GL_FLOAT, false, sizeof(GizmoVertex), (void*)offsetof(GizmoVertex, Color));
}
Gizmos::~Gizmos()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);
}

void Gizmos::Init()
{
    if (Instance == nullptr)
    {
        Instance = new Gizmos();
    }
}
void Gizmos::Destroy()
{
    if (Instance != nullptr)
    {
        delete Instance;
        Instance = nullptr;
    }
}

void Gizmos::Clear()
{
    Instance->m_vertices.clear();
    Instance->m_indices.clear();
}
void Gizmos::DrawAll()
{
    const unsigned int indexCount = Instance->m_indices.size();
    const unsigned int vertexCount = Instance->m_vertices.size();
    if (indexCount > 0 && vertexCount > 0)
    {
        glDisable(GL_CULL_FACE);  
        glDisable(GL_DEPTH_TEST);
    
        const unsigned int programHandle = Instance->m_program->GetHandle();
        glUseProgram(programHandle);
    
        glBindVertexArray(Instance->m_vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, Instance->m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GizmoVertex), &Instance->m_vertices[0], GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Instance->m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &Instance->m_indices[0], GL_STATIC_DRAW);
    
        const glm::mat4 proj = glm::ortho(-1, 1, -1, 1);
    
        glUniformMatrix4fv(0, 1, false, (float*)&proj);
    
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
}
void Gizmos::DrawAll(const Camera* a_camera, const glm::vec2& a_winSize)
{
    const unsigned int indexCount = Instance->m_indices.size();
    const unsigned int vertexCount = Instance->m_vertices.size();
    if (indexCount > 0 && vertexCount > 0)
    {
        glDisable(GL_CULL_FACE);  
        glDisable(GL_DEPTH_TEST);
    
        const unsigned int programHandle = Instance->m_program->GetHandle();
        glUseProgram(programHandle);
    
        glBindVertexArray(Instance->m_vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, Instance->m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GizmoVertex), &Instance->m_vertices[0], GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Instance->m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &Instance->m_indices[0], GL_STATIC_DRAW);
    
        const glm::mat4 view = a_camera->GetView();
        const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);
    
        const glm::mat4 viewProj = proj * view;
    
        glUniformMatrix4fv(0, 1, false, (float*)&viewProj);
    
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
}

void Gizmos::DrawModel(const glm::mat4& a_transform, const Vertex* a_vertices, unsigned int a_vertexCount, const unsigned int* a_indices, unsigned int a_indexCount, const glm::vec4& a_color)
{
    Instance->m_vertices.reserve(a_vertexCount);
    Instance->m_indices.reserve(a_indexCount);

    const unsigned int startIndex = Instance->m_vertices.size();

    for (unsigned int i = 0; i < a_vertexCount; ++i)
    {
        Instance->m_vertices.emplace_back(GizmoVertex{ a_transform * a_vertices[i].Position, a_color });
    }
    for (unsigned int i = 0; i < a_indexCount; ++i)
    {
        Instance->m_indices.emplace_back(a_indices[i] + startIndex);
    }
}
void Gizmos::DrawModel(const glm::mat4& a_transform, const LocalModel* a_model, const glm::vec4& a_color)
{
    const unsigned int vertexCount = a_model->GetVertexCount();
    const Vertex* vertices = a_model->GetVertices();
    const unsigned int indexCount = a_model->GetIndexCount();
    const unsigned int* indicies = a_model->GetIndices();

    DrawModel(a_transform, vertices, vertexCount, indicies, indexCount, a_color);    
}

void Gizmos::DrawLine(const glm::vec3& a_start, const glm::vec3& a_end, float a_width, const glm::vec4& a_color)
{
    if (a_end == a_start)
    {
        return;
    }

    const glm::vec3 forward = glm::normalize(a_end - a_start);

    glm::vec3 up = glm::vec3(0, 1, 0);
    if (glm::abs(glm::dot(up, forward)) >= 0.95f)
    {
        up = glm::vec3(0, 0, 1);
    } 

    const glm::vec3 right = glm::cross(up, forward);

    const unsigned int indexA = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_start + right * a_width, 1.0f), a_color });
    const unsigned int indexB = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_start - right * a_width, 1.0f), a_color });
    const unsigned int indexC = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_end + right * a_width, 1.0f), a_color });
    const unsigned int indexD = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_end - right * a_width, 1.0f), a_color });

    Instance->m_indices.emplace_back(indexA);
    Instance->m_indices.emplace_back(indexC);
    Instance->m_indices.emplace_back(indexB);

    Instance->m_indices.emplace_back(indexB);
    Instance->m_indices.emplace_back(indexC);
    Instance->m_indices.emplace_back(indexD);
}
void Gizmos::DrawLine(const glm::vec3& a_start, const glm::vec3& a_end, const glm::vec3& a_up, float a_width, const glm::vec4& a_color)
{
    if (a_end == a_start)
    {
        return;
    }

    const glm::vec3 forward = glm::normalize(a_end - a_start);

    const glm::vec3 right = glm::cross(a_up, forward);

    const unsigned int indexA = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_start + right * a_width, 1.0f), a_color });
    const unsigned int indexB = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_start - right * a_width, 1.0f), a_color });
    const unsigned int indexC = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_end + right * a_width, 1.0f), a_color });
    const unsigned int indexD = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_end - right * a_width, 1.0f), a_color });

    Instance->m_indices.emplace_back(indexA);
    Instance->m_indices.emplace_back(indexC);
    Instance->m_indices.emplace_back(indexB);

    Instance->m_indices.emplace_back(indexB);
    Instance->m_indices.emplace_back(indexC);
    Instance->m_indices.emplace_back(indexD);
}

void Gizmos::DrawCurve(int a_steps, const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB, const glm::vec4& a_color)
{
    DrawCurve(a_steps, glm::identity<glm::mat4>(), a_nodeA, a_nodeB, a_color);
}
void Gizmos::DrawCurve(int a_steps, const glm::mat4& a_modelMatrix, const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB, const glm::vec4& a_color)
{
    for (int i = 0; i < a_steps; ++i)
    {
        const glm::vec4 pointA = a_modelMatrix * glm::vec4(BezierCurveNode3::GetPoint(a_nodeA, a_nodeB, (float)i / a_steps), 1);
        const glm::vec4 pointB = a_modelMatrix * glm::vec4(BezierCurveNode3::GetPoint(a_nodeA, a_nodeB, (float)(i + 1) / a_steps), 1);

        Gizmos::DrawLine(pointA, pointB, 0.0025f, a_color);
    }
}

void Gizmos::DrawCircle(const glm::vec3& a_position, const glm::vec3& a_dir, float a_radius, float a_width, int a_steps, const glm::vec4& a_color)
{
    const glm::vec4 vec = glm::vec4(0, a_radius, 0, 1);

    for (int i = 0; i < a_steps; ++i)
    {
        const float angleA = (float)i / a_steps * (glm::pi<float>() * 2.0f);
        const float angleB = (float)(i + 1) / a_steps * (glm::pi<float>() * 2.0f);

        const glm::quat qA = glm::angleAxis(angleA, a_dir);
        const glm::quat qB = glm::angleAxis(angleB, a_dir);

        DrawLine(a_position + (qA * vec).xyz(), a_position + (qB * vec).xyz(), a_dir, a_width, a_color);
    }
}
void Gizmos::DrawCircleFilled(const glm::vec3& a_position, const glm::vec3& a_dir, float a_radius, int a_steps, const glm::vec4& a_color)
{
    const unsigned int centerIndex = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position, 1.0f), a_color });

    constexpr float pi2 = glm::pi<float>() * 2;

    glm::vec4 vec = glm::vec4(0.0f, a_radius, 0.0f, 1.0f);
    if (glm::abs(glm::dot(a_dir, glm::vec3(0.0f, 1.0f, 0.0f))) >= 0.95f)
    {
        vec = glm::vec4(0.0f, 0.0f, a_radius, 1.0f);
    }

    glm::quat q = glm::angleAxis(pi2, a_dir);
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position + (q * vec).xyz(), 1.0f), a_color });

    for (int i = 1; i <= a_steps; ++i)
    {
        const float angle = (float)i / a_steps * pi2;

        q = glm::angleAxis(angle, a_dir);
        
        unsigned int index = Instance->m_vertices.size();
        Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position + (q * vec).xyz(), 1.0f), a_color });

        Instance->m_indices.emplace_back(centerIndex);
        Instance->m_indices.emplace_back(index - 1);
        Instance->m_indices.emplace_back(index);
    }
}

void Gizmos::DrawTriangle(const glm::vec3& a_position, const glm::vec3& a_dir, float a_size, const glm::vec4& a_color)
{
    glm::vec3 up = glm::vec3(0, 1, 0);
    if (glm::abs(glm::dot(up, a_dir)) >= 0.95f)
    {
        up = glm::vec3(0, 0, 1);
    } 

    const glm::vec3 right = glm::cross(up, a_dir);

    const float halfSize = a_size * 0.5f;

    const unsigned int indexA = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position + right * halfSize, 1.0f), a_color });
    const unsigned int indexB = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position - right * halfSize, 1.0f), a_color });
    const unsigned int indexC = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position + a_dir * a_size, 1.0f), a_color });

    Instance->m_indices.emplace_back(indexA);
    Instance->m_indices.emplace_back(indexB);
    Instance->m_indices.emplace_back(indexC);
}
void Gizmos::DrawTriangle(const glm::vec3& a_position, const glm::vec3& a_dir, const glm::vec3& a_up, float a_size, const glm::vec4& a_color)
{
    const glm::vec3 right = glm::cross(a_up, a_dir);

    const float halfSize = a_size * 0.5f;

    const unsigned int indexA = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position + right * halfSize, 1.0f), a_color });
    const unsigned int indexB = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position - right * halfSize, 1.0f), a_color });
    const unsigned int indexC = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position + a_dir * a_size, 1.0f), a_color });

    Instance->m_indices.emplace_back(indexA);
    Instance->m_indices.emplace_back(indexB);
    Instance->m_indices.emplace_back(indexC);
}

void Gizmos::DrawTranslation(const glm::vec3& a_position, const glm::vec3& a_dir, float a_scale)
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();

    const glm::mat4 transform = glm::translate(iden, a_position) * glm::scale(iden, glm::vec3(a_scale));

    const LocalModel* handle = TransformVisualizer::GetTranslationHandle();

    constexpr float halfPi = glm::pi<float>() * 0.5f;

    const glm::mat4 rightTransform = transform * glm::toMat4(glm::angleAxis(-halfPi, glm::vec3(0, 0, 1)));
    const glm::mat4 upTransform = transform;
    const glm::mat4 forwardTransform = transform * glm::toMat4(glm::angleAxis(halfPi, glm::vec3(1, 0, 0)));

    Gizmos::DrawModel(glm::translate(iden, glm::vec3(a_scale, 0, 0)) * rightTransform, handle, glm::vec4(1, 0, 0, 1));
    Gizmos::DrawModel(glm::translate(iden, glm::vec3(0, a_scale, 0)) * upTransform, handle, glm::vec4(0, 1, 0, 1));
    Gizmos::DrawModel(glm::translate(iden, glm::vec3(0, 0, a_scale)) * forwardTransform, handle, glm::vec4(0, 0, 1, 1));

    const glm::vec3 endX = a_position + glm::vec3(a_scale, 0, 0);
    const glm::vec3 endY = a_position + glm::vec3(0, a_scale, 0);
    const glm::vec3 endZ = a_position + glm::vec3(0, 0, a_scale);

    Gizmos::DrawLine(a_position, endX, a_dir, 0.01f, glm::vec4(1, 0, 0, 1));
    Gizmos::DrawLine(a_position, endY, a_dir, 0.01f, glm::vec4(0, 1, 0, 1));
    Gizmos::DrawLine(a_position, endZ, a_dir, 0.01f, glm::vec4(0, 0, 1, 1));
}
void Gizmos::DrawScale(const glm::vec3& a_position, const glm::vec3& a_dir, float a_scale)
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();

    const glm::mat4 transform = glm::translate(iden, a_position) * glm::scale(iden, glm::vec3(a_scale));

    const LocalModel* handle = TransformVisualizer::GetScaleHandle();

    constexpr float halfPi = glm::pi<float>() * 0.5f;

    const glm::mat4 rightTransform = transform * glm::toMat4(glm::angleAxis(-halfPi, glm::vec3(0, 0, 1)));
    const glm::mat4 upTransform = transform;
    const glm::mat4 forwardTransform = transform * glm::toMat4(glm::angleAxis(halfPi, glm::vec3(1, 0, 0)));

    Gizmos::DrawModel(glm::translate(iden, glm::vec3(a_scale, 0, 0)) * rightTransform, handle, glm::vec4(1, 0, 0, 1));
    Gizmos::DrawModel(glm::translate(iden, glm::vec3(0, a_scale, 0)) * upTransform, handle, glm::vec4(0, 1, 0, 1));
    Gizmos::DrawModel(glm::translate(iden, glm::vec3(0, 0, a_scale)) * forwardTransform, handle, glm::vec4(0, 0, 1, 1));

    const glm::vec3 endX = a_position + glm::vec3(a_scale, 0, 0);
    const glm::vec3 endY = a_position + glm::vec3(0, a_scale, 0);
    const glm::vec3 endZ = a_position + glm::vec3(0, 0, a_scale);

    Gizmos::DrawLine(a_position, endX, a_dir, 0.01f, glm::vec4(1, 0, 0, 1));
    Gizmos::DrawLine(a_position, endY, a_dir, 0.01f, glm::vec4(0, 1, 0, 1));
    Gizmos::DrawLine(a_position, endZ, a_dir, 0.01f, glm::vec4(0, 0, 1, 1));
}
void Gizmos::DrawRotation(const glm::vec3& a_position, const glm::vec3& a_dir, float a_scale)
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();

    const glm::mat4 transform = glm::translate(iden, a_position) * glm::scale(iden, glm::vec3(a_scale));

    constexpr float halfPi = glm::pi<float>() * 0.5f;

    const glm::mat4 rightTransform = transform * glm::toMat4(glm::angleAxis(-halfPi, glm::vec3(0, 0, 1)));
    const glm::mat4 upTransform = transform;
    const glm::mat4 forwardTransform = transform * glm::toMat4(glm::angleAxis(halfPi, glm::vec3(1, 0, 0)));

    const LocalModel* handle = TransformVisualizer::GetRotationHandle();

    Gizmos::DrawModel(rightTransform, handle, glm::vec4(1, 0, 0, 1));
    Gizmos::DrawModel(upTransform, handle, glm::vec4(0, 1, 0, 1));
    Gizmos::DrawModel(forwardTransform, handle, glm::vec4(0, 0, 1, 1));
}