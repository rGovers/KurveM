#include "PathModel.h"

#include "MeshExporter.h"
#include "Model.h"
#include "XMLIO.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <unordered_map>

PathModel::PathModel(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_model = nullptr;

    m_pathNodes = nullptr;
    m_pathLines = nullptr;

    m_shapeNodes = nullptr;
    m_shapeLines = nullptr;

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
    if (m_pathLines != nullptr)
    {
        delete[] m_pathLines;
        m_pathLines = nullptr;
    }

    if (m_shapeNodes != nullptr)
    {
        delete[] m_shapeNodes;
        m_shapeNodes = nullptr;
    }
    if (m_shapeLines != nullptr)
    {
        delete[] m_shapeLines;
        m_shapeLines = nullptr;
    }

    if (m_model != nullptr)
    {
        delete m_model;
        m_model = nullptr;
    }
}

void PathModel::EmplacePathNodes(const PathNodeCluster* a_nodes, unsigned int a_nodeCount)
{
    const unsigned int size = m_pathNodeCount + a_nodeCount;

    PathNodeCluster* newNodes = new PathNodeCluster[size];

    if (m_pathNodes != nullptr)
    {
        for (unsigned int i = 0; i < m_pathNodeCount; ++i)
        {
            newNodes[i] = m_pathNodes[i];
        }

        delete[] m_pathNodes;
        m_pathNodes = nullptr;
    }

    for (unsigned int i = 0; i < a_nodeCount; ++i)
    {
        newNodes[i + m_pathNodeCount] = a_nodes[i];
    }

    m_pathNodes = newNodes;
    m_pathNodeCount = size;
}
void PathModel::DestroyPathNodes(unsigned int a_startIndex, unsigned int a_endIndex)
{
    const unsigned int count = a_endIndex - a_startIndex;
    const unsigned int size = m_pathNodeCount - count;
    const unsigned int endCount = m_pathNodeCount - a_endIndex;

    if (m_pathNodes != nullptr)
    {
        // Just doing inplace cause it is a shrink operation
        // There is potential that I made need to delete and recreate if memory becomes an issue for some reason but that is easily addressable if needed
        for (unsigned int i = 0; i < endCount; ++i)
        {
            m_pathNodes[i + a_startIndex] = m_pathNodes[i + a_endIndex];
        }
    }

    m_pathNodeCount = size;
}

void PathModel::EmplacePathLine(const PathLine& a_line)
{
    EmplacePathLines(&a_line, 1);
}
void PathModel::EmplacePathLines(const PathLine* a_lines, unsigned int a_lineCount)
{
    const unsigned int size = m_pathLineCount + a_lineCount;

    PathLine* newLines = new PathLine[size];

    if (m_pathLines != nullptr)
    {
        for (unsigned int i = 0; i < m_pathLineCount; ++i)
        {
            newLines[i] = m_pathLines[i];
        }

        delete[] m_pathLines;
        m_pathLines = nullptr;
    }

    for (unsigned int i = 0; i < a_lineCount; ++i)
    {
        newLines[i + m_pathLineCount] = a_lines[i];
    }

    m_pathLines = newLines;
    m_pathLineCount = size;
}
void PathModel::DestroyPathLine(unsigned int a_index)
{
    DestroyPathLines(a_index, a_index + 1);
}
void PathModel::DestroyPathLines(unsigned int a_startIndex, unsigned int a_endIndex)
{
    const unsigned int count  = a_endIndex - a_startIndex;
    const unsigned int size = m_pathLineCount - count;
    const unsigned int endCount = m_pathLineCount - a_endIndex;

    if (m_pathLines != nullptr)
    {
        for (unsigned int i = 0; i < endCount; ++i)
        {
            m_pathNodes[i + a_startIndex] = m_pathNodes[i + a_endIndex];
        }
    }

    m_pathLineCount = size;
}

void PathModel::EmplaceShapeNodes(const ShapeNodeCluster* a_nodes, unsigned int a_nodeCount)
{
    const unsigned int size = m_shapeNodeCount + a_nodeCount;

    ShapeNodeCluster* newNodes = new ShapeNodeCluster[size];

    if (m_shapeNodes != nullptr)
    {
        for (unsigned int i = 0; i < m_shapeNodeCount; ++i)
        {
            newNodes[i] = m_shapeNodes[i];
        }

        delete[] m_shapeNodes;
        m_shapeNodes = nullptr;
    }

    for (unsigned int i = 0; i < a_nodeCount; ++i)
    {
        newNodes[i + m_shapeNodeCount] = a_nodes[i];
    }

    m_shapeNodes = newNodes;
    m_shapeNodeCount = size;
}
void PathModel::DestroyShapeNodes(unsigned int a_startIndex, unsigned int a_endIndex)
{
    const unsigned int count = a_endIndex - a_startIndex;
    const unsigned int size = m_shapeNodeCount - count;
    const unsigned int endCount = m_shapeNodeCount - a_endIndex;

    if (m_shapeNodes != nullptr)
    {
        for (unsigned int i = 0; i < endCount; ++i)
        {
            m_shapeNodes[i + a_startIndex] = m_shapeNodes[i + a_endIndex];
        }
    }

    m_shapeNodeCount = size;
}

void PathModel::EmplaceShapeLines(const ShapeLine* a_lines, unsigned int a_lineCount)
{
    const unsigned int size = m_shapeLineCount + a_lineCount;

    ShapeLine* newLines = new ShapeLine[size];

    if (m_shapeLines != nullptr)
    {
        for (unsigned int i = 0; i < m_shapeLineCount; ++i)
        {
            newLines[i] = m_shapeLines[i];
        }

        delete[] m_shapeLines;
        m_shapeLines = nullptr;
    }

    for (unsigned int i = 0; i < a_lineCount; ++i)
    {
        newLines[i + m_shapeLineCount] = a_lines[i];
    }

    m_shapeLines = newLines;
    m_shapeLineCount = size;
}
void PathModel::DestroyShapeLines(unsigned int a_startIndex, unsigned int a_endIndex)
{
    const unsigned int count = a_endIndex - a_startIndex;
    const unsigned int size = m_shapeLineCount - count;
    const unsigned int endCount = m_shapeLineCount - a_endIndex;

    if (m_shapeLines != nullptr)
    {
        for (unsigned int i = 0; i < endCount; ++i)
        {
            m_shapeLines[i + a_startIndex] = m_shapeLines[i + a_endIndex];
        }
    }

    m_shapeLineCount = size;
}

void PathModel::SetModelData(const PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, const PathLine* a_pathLines, unsigned int a_pathLineCount, 
    const ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, const ShapeLine* a_shapeLines, unsigned int a_shapeLineCount)
{
    SetPathModelData(a_pathNodes, a_pathNodeCount, a_pathLines, a_pathLineCount);
    SetShapeModelData(a_shapeNodes, a_shapeNodeCount, a_shapeLines, a_shapeLineCount);    
}
void PathModel::PassModelData(PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, PathLine* a_pathLines, unsigned int a_pathLineCount,
    ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, ShapeLine* a_shapeLines, unsigned int a_shapeLineCount)
{
   PassPathModelData(a_pathNodes, a_pathNodeCount, a_pathLines, a_pathLineCount);
   PassShapeModelData(a_shapeNodes, a_shapeNodeCount, a_shapeLines, a_shapeLineCount);
}

void PathModel::SetPathModelData(const PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, const PathLine* a_pathLines, unsigned int a_pathLineCount)
{
    if (m_pathNodes != nullptr)
    {
        delete[] m_pathNodes;
        m_pathNodes = nullptr;
    }
    if (m_pathLines != nullptr)
    {
        delete[] m_pathLines;
        m_pathLines = nullptr;
    }

    m_pathNodeCount = a_pathNodeCount;
    m_pathLineCount = a_pathLineCount;

    m_pathNodes = new PathNodeCluster[m_pathNodeCount];
    for (unsigned int i = 0; i < m_pathNodeCount; ++i)
    {
        m_pathNodes[i] = a_pathNodes[i];
    }

    m_pathLines = new PathLine[m_pathLineCount];
    for (unsigned int i = 0; i < m_pathLineCount; ++i)
    {
        m_pathLines[i] = a_pathLines[i];
    }
}
void PathModel::PassPathModelData(PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, PathLine* a_pathLines, unsigned int a_pathLineCount)
{
    m_pathNodeCount = a_pathNodeCount;
    m_pathLineCount = a_pathLineCount;

    m_pathNodes = a_pathNodes;
    m_pathLines = a_pathLines;
}

void PathModel::SetShapeModelData(const ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, const ShapeLine* a_shapeLines, unsigned int a_shapeLineCount)
{
    if (m_shapeNodes != nullptr)
    {
        delete[] m_shapeNodes;
        m_shapeNodes = nullptr;
    }
    if (m_shapeLines != nullptr)
    {
        delete[] m_shapeLines;
        m_shapeLines = nullptr;
    }

    m_shapeNodeCount = a_shapeNodeCount;
    m_shapeLineCount = a_shapeLineCount;

    m_shapeNodes = new ShapeNodeCluster[m_shapeNodeCount];
    for (unsigned int i = 0; i < m_shapeNodeCount; ++i)
    {
        m_shapeNodes[i] = a_shapeNodes[i];
    }

    m_shapeLines = new ShapeLine[m_shapeLineCount];
    for (unsigned int i = 0; i < m_shapeLineCount; ++i)
    {
        m_shapeLines[i] = a_shapeLines[i];
    }
}
void PathModel::PassShapeModelData(ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, ShapeLine* a_shapeLines, unsigned int a_shapeLineCount)
{
    m_shapeNodeCount = a_shapeNodeCount;
    m_shapeLineCount = a_shapeLineCount;

    m_shapeNodes = a_shapeNodes;
    m_shapeLines = a_shapeLines;
}

void PathModel::GetModelData(int a_shapeSteps, int a_pathSteps, unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();

    const unsigned int shapeIndexCount = m_shapeLineCount * a_shapeSteps * 2;

    const unsigned int innerShapeStep = a_shapeSteps - 1;
    const float step = 1.0f / a_shapeSteps;

    std::unordered_map<unsigned int, unsigned int> sIndexMap;
    std::vector<Vertex> shapeVertices;
    unsigned int* shapeIndices = new unsigned int[shapeIndexCount];
    unsigned int index = 0;
    for (unsigned int i = 0; i < m_shapeLineCount; ++i)
    {
        const unsigned int indexA = m_shapeLines[i].Index[0];
        const unsigned int indexB = m_shapeLines[i].Index[1];

        const unsigned char clusterIndexA = m_shapeLines[i].ClusterIndex[0];
        const unsigned char clusterIndexB = m_shapeLines[i].ClusterIndex[1];

        const BezierCurveNode2& nodeA = m_shapeNodes[indexA].Nodes[clusterIndexA];
        const BezierCurveNode2& nodeB = m_shapeNodes[indexB].Nodes[clusterIndexB];

        unsigned int lastIndex;

        auto iter = sIndexMap.find(indexA);
        if (iter != sIndexMap.end())
        {
            lastIndex = iter->second;
        }
        else
        {
            lastIndex = (unsigned int)shapeVertices.size();

            const glm::vec2 pos = nodeA.GetPosition();
            const glm::vec2 nextPos = BezierCurveNode2::GetPoint(nodeA, nodeB, step);

            const glm::vec2 forward = glm::normalize(nextPos - pos);

            sIndexMap.emplace(indexA, lastIndex);

            shapeVertices.emplace_back(Vertex{ glm::vec4(pos.x, 0.0f, pos.y, 1.0f), glm::vec3(-forward.y, 0.0f, forward.x) });
        }

        for (unsigned int j = 0; j < innerShapeStep; ++j)
        {
            const float lerp = (j + 1) / (float)a_shapeSteps;
            const float nextLerp = (j + 2) / (float)a_shapeSteps;

            const glm::vec2 pos = BezierCurveNode2::GetPoint(nodeA, nodeB, lerp);
            const glm::vec2 nextPos = BezierCurveNode2::GetPoint(nodeA, nodeB, nextLerp);

            const glm::vec2 forward = glm::normalize(nextPos - pos);

            const unsigned int curIndex = (unsigned int)shapeVertices.size();

            shapeIndices[index++] = lastIndex;
            shapeIndices[index++] = curIndex;

            lastIndex = curIndex;

            shapeVertices.emplace_back(Vertex{ glm::vec4(pos.x, 0.0f, pos.y, 1.0f), glm::vec3(-forward.y, 0.0f, forward.x) });
        }

        iter = sIndexMap.find(indexB);
        if (iter != sIndexMap.end())
        {
            shapeIndices[index++] = lastIndex;
            shapeIndices[index++] = iter->second;
        }
        else
        {
            const unsigned int curIndex = (unsigned int)shapeVertices.size();

            const glm::vec2 pos = nodeB.GetPosition();
            const glm::vec2 nextPos = BezierCurveNode2::GetPoint(nodeA, nodeB, 1.0f + step);

            const glm::vec2 forward = glm::normalize(nextPos - pos);

            sIndexMap.emplace(indexB, curIndex);

            shapeIndices[index++] = lastIndex;
            shapeIndices[index++] = curIndex;

            shapeVertices.emplace_back(Vertex{ glm::vec4(pos.x, 0.0f, pos.y, 1.0f), glm::vec3(-forward.y, 0.0f, forward.x) });
        }
    }

    const unsigned int shapeVertexCount = shapeVertices.size();
    const unsigned int dirtyVertexCount = m_pathLineCount * (a_pathSteps + 1) * shapeVertexCount; 

    Vertex* dirtyVertices = new Vertex[dirtyVertexCount];
    index = 0;
    for (unsigned int i = 0; i < m_pathLineCount; ++i)
    {
        const unsigned int indexA = m_pathLines[i].Index[0];
        const unsigned int indexB = m_pathLines[i].Index[1];

        const unsigned char clusterIndexA = m_pathLines[i].ClusterIndex[0];
        const unsigned char clusterIndexB = m_pathLines[i].ClusterIndex[1];

        const PathNode& nodeA = m_pathNodes[indexA].Nodes[clusterIndexA];
        const PathNode& nodeB = m_pathNodes[indexB].Nodes[clusterIndexB];

        const PathNode& sNodeA = m_pathNodes[indexA].Nodes[0];
        const PathNode& sNodeB = m_pathNodes[indexB].Nodes[0];

        const glm::vec3 nDA = nodeA.Node.GetHandlePosition() - nodeA.Node.GetPosition();
        const glm::vec3 nDB = nodeB.Node.GetHandlePosition() - nodeB.Node.GetPosition();

        const glm::vec3 nSDA = sNodeA.Node.GetHandlePosition() - sNodeA.Node.GetPosition();
        const glm::vec3 nSDB = sNodeB.Node.GetHandlePosition() - sNodeB.Node.GetPosition();

        const float sRA = glm::sign(glm::dot(nDA, nSDA));
        const float sRB = glm::sign(glm::dot(nDB, nSDB));

        for (unsigned int j = 0; j <= a_pathSteps; ++j)
        {
            const float lerp = (j + 0U) / (float)a_pathSteps;
            const float nextLerp = (j + 1U) / (float)a_pathSteps;

            const glm::vec3 pos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, lerp);
            const glm::vec3 nextPos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, nextLerp);

            const glm::vec3 forward = glm::normalize(nextPos - pos);
            
            const float rot = glm::mix(nodeA.Rotation * sRA, nodeB.Rotation * sRB, lerp);

            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            if (glm::abs(glm::dot(up, forward)) >= 0.95f)
            {
                up = glm::vec3(0.0f, 0.0f, 1.0f);
            }
            // Need to apply rotation around the axis to allow the rotation property to work this should theoretically allow it
            const glm::vec3 right = glm::normalize(glm::angleAxis(rot, forward) * glm::cross(up, forward));
            up = glm::cross(forward, right);

            // Using a rotation matrix because quaternions do not work well with a axis with no rotation
            // Deliberatly mixing axis to correct perspective
            // Forward axis is relative to the next curve point
            const glm::mat3 rotMat = glm::mat3(right, forward, up);

            const glm::vec2 scale = glm::mix(nodeA.Scale, nodeB.Scale, lerp);

            const glm::mat4 mat = glm::translate(iden, pos) * glm::mat4(rotMat) * glm::scale(iden, glm::vec3(scale.x, 1.0f, scale.y));

            for (unsigned int k = 0; k < shapeVertexCount; ++k)
            {
                const Vertex& sVert = shapeVertices[k];
                const glm::vec4 pos = mat * sVert.Position; 
                const glm::vec3 norm = rotMat * sVert.Normal;

                dirtyVertices[index++] = Vertex{ pos, norm };
            }
        }
    }

    std::unordered_map<unsigned int, unsigned int> indexMap;
    
    std::vector<Vertex> vertices;
    vertices.reserve(dirtyVertexCount);
    for (unsigned int i = 0; i < dirtyVertexCount; ++i)
    {
        const unsigned int size = (unsigned int)vertices.size();
        const Vertex& vert = dirtyVertices[i];

        for (unsigned int j = 0; j < size; ++j)
        {
            Vertex& cVert = vertices[j];

            const glm::vec3 diff = vert.Position - cVert.Position;

            if (glm::dot(diff, diff) <= 0.001f)
            {
                cVert.Normal += vert.Normal;
                indexMap.emplace(i, j);

                goto Next;
            }
        }

        indexMap.emplace(i, size);
        vertices.emplace_back(vert);
Next:;
    }

    delete[] dirtyVertices;

    *a_vertexCount = vertices.size();
    *a_vertices = new Vertex[*a_vertexCount];

    for (unsigned int i = 0; i < *a_vertexCount; ++i)
    {
        Vertex& vert = vertices[i];
        vert.Normal = glm::normalize(vert.Normal);
        (*a_vertices)[i] = vert;
    }

    std::vector<unsigned int> indices;

    const unsigned int pathSize = shapeVertexCount * (a_pathSteps + 1);
    const unsigned int shapeLines = shapeIndexCount / 2;
    for (unsigned int i = 0; i < m_pathLineCount; ++i)
    {
        const unsigned int pathIndex = i * pathSize;
        for (unsigned int j = 0; j < shapeLines; ++j)
        {
            const unsigned int jS = j * 2;
            const unsigned int sIndexA = shapeIndices[jS + 0];
            const unsigned int sIndexB = shapeIndices[jS + 1];

            for (unsigned int k = 0; k < a_pathSteps; ++k)
            {
                const unsigned int lA = (k + 0) * shapeVertexCount;
                const unsigned int lB = (k + 1) * shapeVertexCount;

                const unsigned int indexA = indexMap[pathIndex + lA + sIndexA];
                const unsigned int indexB = indexMap[pathIndex + lA + sIndexB];
                const unsigned int indexC = indexMap[pathIndex + lB + sIndexA];
                const unsigned int indexD = indexMap[pathIndex + lB + sIndexB];

                if (indexA == indexD || indexB == indexC || (indexA == indexC && indexB == indexD))
                {
                    continue;
                }

                // Points merged makes a Tri
                if (indexA == indexB || indexA == indexC)
                {
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);
                    indices.emplace_back(indexD);

                    indices.emplace_back(indexB);
                    indices.emplace_back(indexD);
                    indices.emplace_back(indexC);
                }
                // Points merged makes a Tri
                else if (indexD == indexC || indexD == indexB)
                {
                    indices.emplace_back(indexA);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexC);
                    indices.emplace_back(indexB);
                }
                // Makes a Quad
                else
                {
                    indices.emplace_back(indexA);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexD);
                    indices.emplace_back(indexC);

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexC);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);
                    indices.emplace_back(indexD);
                }
            }       
        }
    }

    delete[] shapeIndices;

    *a_indexCount = indices.size();
    *a_indices = new unsigned int[*a_indexCount];

    for (unsigned int i = 0; i < *a_indexCount; ++i)
    {
        (*a_indices)[i] = indices[i];
    }
}

void PathModel::Triangulate()
{
    unsigned int* indices;
    unsigned int indexCount;
    Vertex* vertices;
    unsigned int vertexCount;

    PreTriangulate(&indices, &indexCount, &vertices, &vertexCount);
    PostTriangulate(indices, indexCount, vertices, vertexCount);

    delete[] vertices;
    delete[] indices;
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
}

void PathModel::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent) const
{
    tinyxml2::XMLElement* rootElement = a_doc->NewElement("PathModel");
    a_parent->InsertEndChild(rootElement);

    tinyxml2::XMLElement* shapeElement = a_doc->NewElement("Shape");
    rootElement->InsertEndChild(shapeElement);
    shapeElement->SetAttribute("Steps", m_shapeSteps);

    tinyxml2::XMLElement* shapeLinesElement = a_doc->NewElement("Lines");
    shapeElement->InsertEndChild(shapeLinesElement);

    for (unsigned int i = 0; i < m_shapeLineCount; ++i)
    {
        const ShapeLine& line = m_shapeLines[i];

        tinyxml2::XMLElement* lineElement = a_doc->NewElement("Line");
        shapeLinesElement->InsertEndChild(lineElement);

        for (int j = 0; j < 2; ++j)
        {
            tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
            lineElement->InsertEndChild(indexElement);
            indexElement->SetText(line.Index[j]);

            tinyxml2::XMLElement* clusterIndexElement = a_doc->NewElement("ClusterIndex");
            lineElement->InsertEndChild(clusterIndexElement);
            clusterIndexElement->SetText((unsigned int)line.ClusterIndex[j]);
        }
    }

    tinyxml2::XMLElement* shapeNodesElement = a_doc->NewElement("Nodes");
    shapeElement->InsertEndChild(shapeNodesElement);

    for (unsigned int i = 0; i < m_shapeNodeCount; ++i)
    {
        const ShapeNodeCluster& node = m_shapeNodes[i];

        const int size = node.Nodes.size();
        if (size > 0)
        {
            tinyxml2::XMLElement* shapeNodeElement = a_doc->NewElement("Node");
            XMLIO::WriteVec2(a_doc, shapeNodeElement, "Position", node.Nodes[0].GetPosition());
            shapeNodesElement->InsertEndChild(shapeNodeElement);

            for (int j = 0; j < size; ++j)
            {
                const BezierCurveNode2& sNode = node.Nodes[j];

                tinyxml2::XMLElement* clusterNodeElement = a_doc->NewElement("ClusterNode");
                shapeNodeElement->InsertEndChild(clusterNodeElement);

                XMLIO::WriteVec2(a_doc, clusterNodeElement, "HandlePosition", sNode.GetHandlePosition());
            }

        }
    }

    tinyxml2::XMLElement* pathElement = a_doc->NewElement("Path");
    rootElement->InsertEndChild(pathElement);
    pathElement->SetAttribute("Steps", m_pathSteps);

    tinyxml2::XMLElement* pathLinesElement = a_doc->NewElement("Lines");
    pathElement->InsertEndChild(pathLinesElement);

    for (unsigned int i = 0; i < m_pathLineCount; ++i)
    {
        const PathLine& line = m_pathLines[i];

        tinyxml2::XMLElement* lineElement = a_doc->NewElement("Line");
        pathLinesElement->InsertEndChild(lineElement);

        for (int j = 0; j < 2; ++j)
        {
            tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
            lineElement->InsertEndChild(indexElement);
            indexElement->SetText(line.Index[j]);

            tinyxml2::XMLElement* clusterIndexElement = a_doc->NewElement("ClusterIndex");
            lineElement->InsertEndChild(clusterIndexElement);
            clusterIndexElement->SetText((unsigned int)line.ClusterIndex[j]);
        }
    }

    tinyxml2::XMLElement* pathNodesElement = a_doc->NewElement("Nodes");
    pathElement->InsertEndChild(pathNodesElement);

    for (unsigned int i = 0; i < m_pathNodeCount; ++i)
    {
        const PathNodeCluster& node = m_pathNodes[i];
        
        const int size = node.Nodes.size();
        if (size > 0)
        {
            tinyxml2::XMLElement* pathNodeElement = a_doc->NewElement("Node");
            pathNodesElement->InsertEndChild(pathNodeElement);

            XMLIO::WriteVec3(a_doc, pathNodeElement, "Position", node.Nodes[0].Node.GetPosition());
            for (int j = 0; j < size; ++j)
            {
                const PathNode& pNode = node.Nodes[j];

                tinyxml2::XMLElement* clusterNodeElement = a_doc->NewElement("ClusterNode");
                pathNodeElement->InsertEndChild(clusterNodeElement);

                XMLIO::WriteVec3(a_doc, clusterNodeElement, "HandlePosition", pNode.Node.GetHandlePosition());

                tinyxml2::XMLElement* rotationElement = a_doc->NewElement("Rotation");
                clusterNodeElement->InsertEndChild(rotationElement);
                rotationElement->SetText(pNode.Rotation);

                XMLIO::WriteVec2(a_doc, clusterNodeElement, "Scale", pNode.Scale, glm::vec2(1.0f));
            }
        }   
    }
}
void PathModel::ParseData(const tinyxml2::XMLElement* a_element)
{
    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();

        if (strcmp(str, "Shape") == 0)
        {
            m_shapeSteps = iter->IntAttribute("Steps", 1);

            for (const tinyxml2::XMLElement* sIter = iter->FirstChildElement(); sIter != nullptr; sIter = sIter->NextSiblingElement())
            {
                const char* sStr = sIter->Value();
                
                if (strcmp(sStr, "Lines") == 0)
                {
                    std::vector<ShapeLine> lines;

                    for (const tinyxml2::XMLElement* lIter = sIter->FirstChildElement(); lIter != nullptr; lIter = lIter->NextSiblingElement())
                    {
                        const char* lStr = lIter->Value();

                        if (strcmp(lStr, "Line") == 0)
                        {
                            ShapeLine line;

                            int index = 0;
                            int cIndex = 0;

                            for (const tinyxml2::XMLElement* iIter = lIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                            {
                                const char* iStr = iIter->Value();
                                if (strcmp(iStr, "Index") == 0)
                                {
                                    line.Index[index++] = iIter->UnsignedText();
                                }
                                else if (strcmp(iStr, "ClusterIndex") == 0)
                                {
                                    line.ClusterIndex[cIndex++] = (unsigned char)iIter->UnsignedText();
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(iStr);
                                    printf("\n");
                                }
                            }

                            lines.emplace_back(line);
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(lStr);
                            printf("\n");
                        }
                    }

                    m_shapeLineCount = lines.size();

                    if (m_shapeLines != nullptr)
                    {
                        delete[] m_shapeLines;
                        m_shapeLines = nullptr;
                    }

                    if (m_shapeLineCount > 0)
                    {
                        m_shapeLines = new ShapeLine[m_shapeLineCount];

                        for (unsigned int i = 0; i < m_shapeLineCount; ++i)
                        {
                            m_shapeLines[i] = lines[i];
                        }
                    }
                }
                else if (strcmp(sStr, "Nodes") == 0)
                {
                    std::vector<ShapeNodeCluster> nodes;

                    for (const tinyxml2::XMLElement* nIter = sIter->FirstChildElement(); nIter != nullptr; nIter = nIter->NextSiblingElement())
                    {
                        const char* nStr = nIter->Value();
                        if (strcmp(nStr, "Node") == 0)
                        {
                            glm::vec2 pos = glm::vec2(0.0f);
                            ShapeNodeCluster c;

                            for (const tinyxml2::XMLElement* niIter = nIter->FirstChildElement(); niIter != nullptr; niIter = niIter->NextSiblingElement())
                            {
                                const char* niStr = niIter->Value();

                                if (strcmp(niStr, "Position") == 0)
                                {
                                    XMLIO::ReadVec2(niIter, &pos);
                                }
                                else if (strcmp(niStr, "ClusterNode") == 0)
                                {
                                    BezierCurveNode2 n;

                                    n.SetPosition(glm::vec2(std::numeric_limits<float>::infinity()));

                                    for (const tinyxml2::XMLElement* cIter = niIter->FirstChildElement(); cIter != nullptr; cIter = cIter->NextSiblingElement())
                                    {
                                        const char* cStr = cIter->Value();

                                        if (strcmp(cStr, "HandlePosition") == 0)
                                        {
                                            n.SetHandlePosition(XMLIO::GetVec2(cIter));
                                        }
                                        else
                                        {
                                            printf("PathModel::ParseData: Invalid Element: ");
                                            printf(cStr);
                                            printf("\n");
                                        }
                                    }

                                    c.Nodes.emplace_back(n);
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(niStr);
                                    printf("\n");
                                }
                            }

                            for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                            {
                                if (iter->GetPosition().x == std::numeric_limits<float>::infinity())
                                {
                                    iter->SetPosition(pos);
                                }
                            }

                            nodes.emplace_back(c);
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(nStr);
                            printf("\n");
                        }
                    }

                    m_shapeNodeCount = nodes.size();

                    if (m_shapeNodes != nullptr)
                    {
                        delete[] m_shapeNodes;
                        m_shapeNodes = nullptr;
                    }

                    if (m_shapeLineCount > 0)
                    {
                        m_shapeNodes = new ShapeNodeCluster[m_shapeNodeCount];

                        for (unsigned int i = 0; i < m_shapeNodeCount; ++i)
                        {
                            m_shapeNodes[i] = nodes[i];
                        }
                    }
                }   
                else
                {
                    printf("PathModel::ParseData: Invalid Element: ");
                    printf(sStr);
                    printf("\n");
                }
            }
        }
        else if (strcmp(str, "Path") == 0)
        {
            m_pathSteps = iter->IntAttribute("Steps");

            for (const tinyxml2::XMLElement* pIter = iter->FirstChildElement(); pIter != nullptr; pIter = pIter->NextSiblingElement())
            {
                const char* pStr = pIter->Value();

                if (strcmp(pStr, "Lines") == 0)
                {
                    std::vector<PathLine> lines;

                    for (const tinyxml2::XMLElement* lIter = pIter->FirstChildElement(); lIter != nullptr; lIter = lIter->NextSiblingElement())
                    {
                        const char* lStr = lIter->Value();

                        if (strcmp(lStr, "Line") == 0)
                        {
                            PathLine line;

                            int index = 0;
                            int cIndex = 0;

                            for (const tinyxml2::XMLElement* iIter = lIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                            {
                                const char* iStr = iIter->Value();

                                if (strcmp(iStr, "Index") == 0)
                                {
                                    line.Index[index++] = iIter->UnsignedText();
                                }
                                else if (strcmp(iStr, "ClusterIndex") == 0)
                                {
                                    line.ClusterIndex[cIndex++] = (unsigned char)iIter->UnsignedText();
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(iStr);
                                    printf("\n");
                                }
                            }

                            lines.emplace_back(line);
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(lStr);
                            printf("\n");
                        }
                    }

                    m_pathLineCount = lines.size();

                    if (m_pathLines != nullptr)
                    {
                        delete[] m_pathLines;
                        m_pathLines = nullptr;
                    }

                    if (m_pathLineCount > 0)
                    {
                        m_pathLines = new PathLine[m_pathLineCount];

                        for (unsigned int i = 0; i < m_pathLineCount; ++i)
                        {
                            m_pathLines[i] = lines[i];
                        }
                    }
                }
                else if (strcmp(pStr, "Nodes") == 0)
                {
                    std::vector<PathNodeCluster> nodes;

                    for (const tinyxml2::XMLElement* nIter = pIter->FirstChildElement(); nIter != nullptr; nIter = nIter->NextSiblingElement())
                    {
                        const char* nStr = nIter->Value();

                        if (strcmp(nStr, "Node") == 0)
                        {
                            PathNodeCluster c;
                            glm::vec3 pos = glm::vec3(0.0f);

                            for (const tinyxml2::XMLElement* niIter = nIter->FirstChildElement(); niIter != nullptr; niIter = niIter->NextSiblingElement())
                            {
                                const char* niStr = niIter->Value();

                                if (strcmp(niStr, "Position") == 0)
                                {
                                    XMLIO::ReadVec3(niIter, &pos);
                                }
                                else if (strcmp(niStr, "ClusterNode") == 0)
                                {
                                    PathNode node;

                                    node.Node.SetPosition(glm::vec3(std::numeric_limits<float>::infinity()));

                                    for (const tinyxml2::XMLElement* cIter = niIter->FirstChildElement(); cIter != nullptr; cIter = cIter->NextSiblingElement())
                                    {
                                        const char* cStr = cIter->Value();

                                        if (strcmp(cStr, "HandlePosition") == 0)
                                        {
                                            node.Node.SetHandlePosition(XMLIO::GetVec3(cIter));
                                        }
                                        else if (strcmp(cStr, "Rotation") == 0)
                                        {
                                            node.Rotation = cIter->FloatText();   
                                        }
                                        else if (strcmp(cStr, "Scale") == 0)
                                        {
                                            node.Scale = XMLIO::GetVec2(cIter, glm::vec2(1.0f));
                                        }
                                        else
                                        {
                                            printf("PathModel::ParseData: Invalid Element: ");
                                            printf(cStr);
                                            printf("\n");
                                        }
                                    }

                                    c.Nodes.emplace_back(node);
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(niStr);
                                    printf("\n");
                                }
                            }

                            for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                            {
                                if (iter->Node.GetPosition().x == std::numeric_limits<float>::infinity())
                                {
                                    iter->Node.SetPosition(pos);
                                }
                            }

                            nodes.emplace_back(c);
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(nStr);
                            printf("\n");
                        }
                    }

                    m_pathNodeCount = nodes.size();

                    if (m_pathNodes != nullptr)
                    {
                        delete[] m_pathNodes;
                        m_pathNodes = nullptr;
                    }

                    if (m_pathNodeCount > 0)
                    {
                        m_pathNodes = new PathNodeCluster[m_pathNodeCount];

                        for (unsigned int i = 0; i < m_pathNodeCount; ++i)
                        {
                            m_pathNodes[i] = nodes[i];
                        }
                    }
                }
                else
                {
                    printf("PathModel::ParseData: Invalid Element: ");
                    printf(pStr);
                    printf("\n");
                }
            }
        }
        else
        {
            printf("PathModel::ParseData: Invalid Element: ");
            printf(str);
            printf("\n");
        }
    }
}

void PathModel::WriteOBJ(std::ofstream* a_file, int a_pathSteps, int a_shapeSteps) const
{
    unsigned int* indices;
    Vertex* vertices;
    unsigned int indexCount;
    unsigned int vertexCount;

    GetModelData(a_shapeSteps, a_pathSteps, &indices, &indexCount, &vertices, &vertexCount);

    MeshExporter::ExportOBJMesh(a_file, vertices, vertexCount, indices, indexCount);

    delete[] vertices;
    delete[] indices;
}
void PathModel::WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_name, int a_pathSteps, int a_shapeSteps) const
{
    tinyxml2::XMLElement* meshElement = a_doc->NewElement("mesh");
    a_parent->InsertEndChild(meshElement);

    Vertex* vertices;
    unsigned int* indices;
    unsigned int indexCount;
    unsigned int vertexCount;

    GetModelData(a_shapeSteps, a_pathSteps, &indices, &indexCount, &vertices, &vertexCount);

    MeshExporter::ExportColladaMesh(a_doc, meshElement, vertices, vertexCount, indices, indexCount, a_name);

    delete[] vertices;
    delete[] indices;
}