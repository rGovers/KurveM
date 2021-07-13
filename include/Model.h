#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Vertex
{
    glm::vec4 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
};

class Model
{
private:
    static Model* InstanceEmpty;

    unsigned int m_vbo;
    unsigned int m_ibo;
    unsigned int m_vao;

    unsigned int m_indices;
    
    Model();
protected:

public:
    Model(Vertex* a_vertices, unsigned int* a_indices, unsigned int a_vertexCount, unsigned int a_indexCount);
    ~Model();

    static void Init();
    static void Destroy();

    static Model* GetEmpty()
    {
        return InstanceEmpty;
    }

    inline unsigned int GetIndexCount() const
    {
        return m_indices;
    }
    inline unsigned int GetVAO() const
    {
        return m_vao;
    }
};