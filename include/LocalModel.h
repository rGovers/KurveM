#pragma once

#include "Model.h"

class LocalModel : public Model
{
private:
    unsigned int* m_indices;

    unsigned int  m_vertexCount;
    Vertex*       m_vertices;
    
protected:

public:
    LocalModel(Vertex* a_vertices, unsigned int* a_indices, unsigned int a_vertexCount, unsigned int a_indexCount);
    virtual ~LocalModel();

    inline unsigned int* GetIndices() const
    {
        return m_indices;
    }
    inline Vertex* GetVertices() const
    {
        return m_vertices;
    }
    inline unsigned int GetVertexCount() const
    {
        return m_vertexCount;
    }
};