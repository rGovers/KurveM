#include "PathModel.h"

#include "Model.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <unordered_map>

PathModel::PathModel(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_model = nullptr;

    m_pathNodes = nullptr;
    m_pathIndices = nullptr;

    m_shapeNodes = nullptr;
    m_shapeIndices = nullptr;

    m_pathNodeCount = 0;
    m_shapeNodes = 0;

    m_shapeSteps = 2;
    m_pathSteps = 5;
}
PathModel::~PathModel()
{
    if (m_pathNodes != nullptr)
    {
        delete[] m_pathNodes;
        m_pathNodes = nullptr;
    }
    if (m_pathIndices != nullptr)
    {
        delete[] m_pathIndices;
        m_pathIndices = nullptr;
    }

    if (m_shapeNodes != nullptr)
    {
        delete[] m_shapeNodes;
        m_shapeNodes = nullptr;
    }
    if (m_shapeIndices != nullptr)
    {
        delete[] m_shapeIndices;
        m_shapeIndices = nullptr;
    }

    if (m_model != nullptr)
    {
        delete m_model;
        m_model = nullptr;
    }
}

void PathModel::SetModelData(PathNode* a_nodes, unsigned int a_nodeCount, unsigned int* a_nodeIndices, unsigned int a_nodeIndexCount, 
    BezierCurveNode2* a_shapeNodes, unsigned int a_shapeNodeCount, unsigned int* a_shapeIndices, unsigned int a_shapeIndexCount)
{
    if (m_pathNodes != nullptr)
    {
        delete[] m_pathNodes;
        m_pathNodes = nullptr;
    }
    if (m_pathIndices != nullptr)
    {
        delete[] m_pathIndices;
        m_pathIndices = nullptr;
    }

    m_pathNodeCount = a_nodeCount;
    m_pathIndexCount = a_nodeIndexCount;

    m_pathNodes = a_nodes;
    m_pathIndices = a_nodeIndices;

    if (m_shapeNodes != nullptr)
    {
        delete[] m_shapeNodes;
        m_shapeNodes = nullptr;
    }
    if (m_shapeIndices != nullptr)
    {
        delete[] m_shapeIndices;
        m_shapeIndices = nullptr;
    }

    m_shapeNodeCount = a_shapeNodeCount;
    m_shapeIndexCount = a_shapeIndexCount;

    m_shapeNodes = a_shapeNodes;
    m_shapeIndices = a_shapeIndices;
}

void PathModel::GetModelData(int a_shapeSteps, int a_pathSteps, unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const
{
    const unsigned int shapeLines = m_shapeIndexCount / 2;

    std::vector<Vertex> shapeVertices;

    for (unsigned int i = 0; i < shapeLines; ++i)
    {
        const BezierCurveNode2 nodeA = m_shapeNodes[m_shapeIndices[i * 2 + 0U]];
        const BezierCurveNode2 nodeB = m_shapeNodes[m_shapeIndices[i * 2 + 1U]];

        for (unsigned int j = 0; j <= a_shapeSteps; ++j)
        {
            const float lerp = (j + 0U) / (float)a_shapeSteps;
            const float nextLerp = (j + 1U) / (float)a_shapeSteps;

            const glm::vec2 pos = BezierCurveNode2::GetPoint(nodeA, nodeB, lerp);
            const glm::vec2 nextPos = BezierCurveNode2::GetPoint(nodeA, nodeB, nextLerp);

            const glm::vec2 forward = glm::normalize(nextPos - pos);

            shapeVertices.emplace_back(Vertex{ glm::vec4(pos.x, 0.0f, pos.y, 1.0f), glm::vec3(-forward.y, 0, forward.x) });
        }
    }

    const unsigned int shapeVertexCount = shapeVertices.size();

    const unsigned int pathLines = m_pathIndexCount / 2;

    const glm::mat4 iden = glm::identity<glm::mat4>();

    std::vector<Vertex> dirtyVertices;
    for (unsigned int i = 0; i < pathLines; ++i)
    {
        const PathNode nodeA = m_pathNodes[m_pathIndices[(i * 2) + 0U]];
        const PathNode nodeB = m_pathNodes[m_pathIndices[(i * 2) + 1U]];

        for (unsigned int j = 0; j <= a_pathSteps; ++j)
        {
            const float lerp = (j + 0U) / (float)a_pathSteps;
            const float nextLerp = (j + 1U) / (float)a_pathSteps;

            const glm::vec3 pos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, lerp);
            const glm::vec3 nextPos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, nextLerp);

            const glm::vec3 forward = glm::normalize(nextPos - pos);
            const float rot = glm::mix(nodeA.Rotation, nodeB.Rotation, lerp);

            const glm::quat qt = glm::angleAxis(rot, forward);

            const glm::vec2 scale = glm::mix(nodeA.Scale, nodeB.Scale, lerp);

            const glm::mat4 mat = glm::scale(iden, glm::vec3(scale.x, 1.0f, scale.y)) * glm::translate(iden, pos) * glm::toMat4(qt);

            for (unsigned int k = 0; k < shapeVertexCount; ++k)
            {
                const glm::vec3 pos = mat * shapeVertices[k].Position; 
                const glm::vec3 norm = qt * shapeVertices[k].Normal;

                dirtyVertices.emplace_back(Vertex{ glm::vec4(pos, 1.0f), norm });
            }
        }
    }

    std::unordered_map<unsigned int, unsigned int> indexMap;

    const unsigned int dirtyVertexCount = dirtyVertices.size(); 

    *a_vertices = new Vertex[dirtyVertexCount];

    unsigned int vertexIndex = 0;

    for (unsigned int i = 0; i < dirtyVertexCount; ++i)
    {
        const Vertex vert = dirtyVertices[i];

        for (unsigned int j = 0; j < vertexIndex; ++j)
        {
            const Vertex cVert = (*a_vertices)[j];

            const glm::vec3 diff = vert.Position - cVert.Position;

            if (glm::dot(diff, diff) <= 0.0001f)
            {
                (*a_vertices)[j].Normal += vert.Normal;
                indexMap.emplace(i, j);

                goto Next;
            }
        }

        (*a_vertices)[vertexIndex] = vert;
        indexMap.emplace(i, vertexIndex++);
Next:;
    }

    *a_vertexCount = vertexIndex;

    for (unsigned int i = 0; i < *a_vertexCount; ++i)
    {
        (*a_vertices)[i].Normal = glm::normalize((*a_vertices)[i].Normal);
    }

    std::vector<unsigned int> indices;

    const unsigned int loops = dirtyVertexCount / shapeVertexCount;

    for (unsigned int i = 0; i < loops - 1; ++i)
    {
        for (unsigned int j = 0; j < shapeVertexCount; ++j)
        {
            const unsigned int indexA = indexMap[(i + 0U) * shapeVertexCount + (j + 0U)];
            const unsigned int indexB = indexMap[(i + 0U) * shapeVertexCount + (j + 1U)];
            const unsigned int indexC = indexMap[(i + 1U) * shapeVertexCount + (j + 0U)];
            const unsigned int indexD = indexMap[(i + 1U) * shapeVertexCount + (j + 1U)];

            indices.emplace_back(indexA);
            indices.emplace_back(indexC);
            indices.emplace_back(indexB);

            indices.emplace_back(indexB);
            indices.emplace_back(indexC);
            indices.emplace_back(indexD);
        }
    }

    *a_indexCount = indices.size();
    *a_indices = new unsigned int[*a_indexCount];

    for (unsigned int i = 0; i < *a_indexCount; ++i)
    {
        (*a_indices)[i] = indices[i];
    }
}

void PathModel::PreTriangulate(unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const
{
    GetModelData(m_shapeSteps, m_pathSteps, a_indices, a_indexCount, a_vertices, a_vertexCount);
}
void PathModel::PostTriangulate(unsigned int* a_indices, unsigned int a_indexCount, Vertex* a_vertices, unsigned int a_vertexCount)
{
    if (m_model != nullptr)
    {
        delete m_model;
        m_model = nullptr;
    }

    if (a_vertexCount != 0 && a_indexCount != 0)
    {
        m_model = new Model(a_vertices, a_indices, a_vertexCount, a_indexCount);
    }

    delete[] a_vertices;
    delete[] a_indices;
}