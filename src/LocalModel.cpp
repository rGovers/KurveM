#include "LocalModel.h"

#include <vector>

LocalModel::LocalModel(Vertex* a_vertices, unsigned int* a_indices, unsigned int a_vertexCount, unsigned int a_indexCount) : 
    Model(a_vertices, a_indices, a_vertexCount, a_indexCount)
{
    m_vertexCount = a_vertexCount;

    m_vertices = new Vertex[m_vertexCount];

    for (unsigned int i = 0; i < m_vertexCount; ++i)
    {
        m_vertices[i] = a_vertices[i];
    }
    
    m_indices = new unsigned int[a_indexCount];

    for (unsigned int i = 0; i < a_indexCount; ++i)
    {
        m_indices[i] = a_indices[i];
    }
}
LocalModel::~LocalModel()
{
    delete[] m_indices;
    delete[] m_vertices;
}