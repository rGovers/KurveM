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
    glm::vec4 BodyI;
    glm::vec4 BodyW;
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
    static constexpr unsigned int PositionBinding = 0;
    static constexpr unsigned int NormalBinding = 1;
    static constexpr unsigned int UVBinding = 2;
    static constexpr unsigned int BoneBinding = 3;
    static constexpr unsigned int WeightBinding = 4;
    static constexpr unsigned int SoftbodyIndexBinding = 5;
    static constexpr unsigned int SoftbodyWeightBinding = 6;

    Model(const Vertex* a_vertices, const unsigned int* a_indices, unsigned int a_vertexCount, unsigned int a_indexCount);
    virtual ~Model();

    static void Init();
    static void Destroy();

    static inline Model* GetEmpty()
    {
        return InstanceEmpty;
    }

    inline unsigned int GetIndexCount() const
    {
        return m_indices;
    }

    inline unsigned int GetVBO() const
    {
        return m_vbo;
    }
    inline unsigned int GetIBO() const
    {
        return m_ibo;
    }
    inline unsigned int GetVAO() const
    {
        return m_vao;
    }
};