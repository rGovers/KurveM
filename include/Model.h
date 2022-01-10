#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec4 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
    glm::vec4 Bones;
    glm::vec4 Weights;
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
    Model(const Vertex* a_vertices, const unsigned int* a_indices, unsigned int a_vertexCount, unsigned int a_indexCount);
    virtual ~Model();

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