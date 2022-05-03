#include "Physics/StridingMeshInterface.h"

#include "LocalModel.h"

StridingMeshInterface::StridingMeshInterface(const LocalModel* a_mesh)
{
    m_mesh = a_mesh;

    m_verts = nullptr;

    const unsigned int vertexCount = m_mesh->GetVertexCount();
    const Vertex* vertices = m_mesh->GetVertices();
    if (vertexCount > 0)
    {
        m_verts = new glm::vec4[vertexCount];

        for (unsigned int i = 0; i < vertexCount; ++i)
        {
            m_verts[i] = vertices[i].Position;
        }
    }
}
StridingMeshInterface::~StridingMeshInterface()
{
    if (m_verts != nullptr)
    {
        delete[] m_verts;
        m_verts = nullptr;
    }
}

void StridingMeshInterface::SetMesh(const LocalModel* a_mesh)
{
    m_mesh = a_mesh;

    if (m_verts != nullptr)
    {
        delete[] m_verts;
        m_verts = nullptr;
    }

    const unsigned int vertexCount = m_mesh->GetVertexCount();
    const Vertex* vertices = m_mesh->GetVertices();
    if (vertexCount > 0)
    {
        m_verts = new glm::vec4[vertexCount];

        for (unsigned int i = 0; i < vertexCount; ++i)
        {
            m_verts[i] = vertices[i].Position;
        }
    }
}

void StridingMeshInterface::getLockedVertexIndexBase(unsigned char** a_vertexbase, int& a_numverts, PHY_ScalarType& a_type, int& a_stride, unsigned char** a_indexbase, int& a_indexstride, int& a_numfaces, PHY_ScalarType& a_indicestype, int a_subpart)
{
    a_numverts = 0;
    a_numfaces = 0;

    a_type = PHY_FLOAT;
    a_stride = (int)sizeof(glm::vec4);

    a_indicestype = PHY_INTEGER;
    a_indexstride = (int)(sizeof(unsigned int) * 3);

    if (m_mesh != nullptr)
    {
        a_numverts = (int)m_mesh->GetVertexCount();
        *a_vertexbase = (unsigned char*)m_verts;
        
        a_numfaces = (int)(m_mesh->GetIndexCount() / 3);
        *a_indexbase = (unsigned char*)m_mesh->GetIndices();
    }
}
void StridingMeshInterface::getLockedReadOnlyVertexIndexBase(const unsigned char** a_vertexbase, int& a_numverts, PHY_ScalarType& a_type, int& a_stride, const unsigned char** a_indexbase, int& a_indexstride, int& a_numfaces, PHY_ScalarType& a_indicestype, int a_subpart) const
{
    a_numverts = 0;
    a_numfaces = 0;

    a_type = PHY_FLOAT;
    a_stride = (int)sizeof(glm::vec4);

    a_indicestype = PHY_INTEGER;
    a_indexstride = (int)(sizeof(unsigned int) * 3);

    if (m_mesh != nullptr)
    {
        a_numverts = (int)m_mesh->GetVertexCount();
        *a_vertexbase = (unsigned char*)m_verts;
        
        a_numfaces = (int)(m_mesh->GetIndexCount() / 3);
        *a_indexbase = (unsigned char*)m_mesh->GetIndices();
    }
}

int StridingMeshInterface::getNumSubParts() const
{
    return 1;
}

void StridingMeshInterface::Tick(const glm::mat4* a_matrices, unsigned int a_matrixCount)
{
    if (m_verts != nullptr)
    {
        delete[] m_verts;
        m_verts = nullptr;
    }

    const unsigned int vertexCount = m_mesh->GetVertexCount();
    const Vertex* vertices = m_mesh->GetVertices();
    if (vertexCount > 0)
    {
        m_verts = new glm::vec4(vertexCount);

        for (unsigned int i = 0; i < vertexCount; ++i)
        {
            const Vertex& vert = vertices[i];

            const glm::vec4& weights = vert.Weights;
            const glm::vec4& bones = vert.Bones;

            const glm::mat4 mat = a_matrices[(unsigned int)(bones.x * a_matrixCount)] * weights.x + 
                a_matrices[(unsigned int)(bones.y * a_matrixCount)] * weights.y +
                a_matrices[(unsigned int)(bones.z * a_matrixCount)] * weights.z +
                a_matrices[(unsigned int)(bones.w * a_matrixCount)] * weights.w;

            m_verts[i] = mat * vert.Position;
        }
    }
}