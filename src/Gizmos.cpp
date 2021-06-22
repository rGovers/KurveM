#include "Gizmos.h"

#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"
#include "Datastore.h"
#include "ShaderPixel.h"
#include "ShaderProgram.h"
#include "Shaders/GizmosPixel.h"
#include "Shaders/GizmosVertex.h"
#include "ShaderVertex.h"

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
void Gizmos::DrawAll(Camera* a_camera, const glm::vec2& a_winSize)
{
    glDisable(GL_CULL_FACE);  

    const unsigned int programHandle = Instance->m_program->GetHandle();
    glUseProgram(programHandle);

    glBindVertexArray(Instance->m_vao);
    
    const unsigned int vertexCount = Instance->m_vertices.size();
    glBindBuffer(GL_ARRAY_BUFFER, Instance->m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GizmoVertex), &Instance->m_vertices[0], GL_STATIC_DRAW);
    
    const unsigned int indexCount = Instance->m_indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Instance->m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &Instance->m_indices[0], GL_STATIC_DRAW);

    const glm::mat4 view = a_camera->GetView();
    const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);

    const glm::mat4 viewProj = proj * view;

    glUniformMatrix4fv(0, 1, false, (float*)&viewProj);

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void Gizmos::DrawLine(const glm::vec3& a_start, const glm::vec3& a_end, float a_width, const glm::vec4& a_color)
{
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

void Gizmos::DrawCircle(const glm::vec3& a_position, const glm::vec3& a_dir, float a_radius, int a_steps, const glm::vec4& a_color)
{
    const glm::vec3 vec = glm::vec3(0, 0, a_radius);

    for (int i = 0; i < a_steps; ++i)
    {
        const float angleA = (float)i / a_steps * (glm::pi<float>() * 2.0f);
        const float angleB = (float)(i + 1) / a_steps * (glm::pi<float>() * 2.0f);

        const glm::quat qA = glm::angleAxis(angleA, a_dir);
        const glm::quat qB = glm::angleAxis(angleB, a_dir);

        DrawLine(a_position + (qA * vec), a_position + (qB * vec), 0.1f, a_color);
    }
}
void Gizmos::DrawCircleFilled(const glm::vec3& a_position, const glm::vec3& a_dir, float a_radius, int a_steps, const glm::vec4& a_color)
{
    const unsigned int centerIndex = Instance->m_vertices.size();
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position, 1.0f), a_color });

    const glm::vec3 vec = glm::vec3(0, 0, a_radius);

    glm::quat q = glm::angleAxis(0.0f, a_dir);
    Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position + (q * vec), 1.0f), a_color });

    for (int i = 1; i <= a_steps; ++i)
    {
        const float angle = (float)i / a_steps * (glm::pi<float>() * 2.0f);

        glm::quat q = glm::angleAxis(angle, a_dir);
        
        unsigned int index = Instance->m_vertices.size();
        Instance->m_vertices.emplace_back(GizmoVertex{ glm::vec4(a_position + (q * vec), 1.0f), a_color });

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