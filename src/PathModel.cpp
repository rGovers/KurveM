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

void PathModel::SetModelData(const PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, const PathLine* a_pathLines, unsigned int a_pathLineCount, 
    const ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, const ShapeLine* a_shapeLines, unsigned int a_shapeLineCount)
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
void PathModel::PassModelData(PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, PathLine* a_pathLines, unsigned int a_pathLineCount,
    ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, ShapeLine* a_shapeLines, unsigned int a_shapeLineCount)
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

    m_pathNodes = a_pathNodes;
    m_pathLines = a_pathLines;

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

    m_shapeNodes = a_shapeNodes;
    m_shapeLines = a_shapeLines;
}

void PathModel::GetModelData(int a_shapeSteps, int a_pathSteps, unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const
{
    std::vector<Vertex> shapeVertices;

    for (unsigned int i = 0; i < m_shapeLineCount; ++i)
    {
        const unsigned int indexA = m_shapeLines[i].Index[0];
        const unsigned int indexB = m_shapeLines[i].Index[1];

        const unsigned char clusterIndexA = m_shapeLines[i].ClusterIndex[0];
        const unsigned char clusterIndexB = m_shapeLines[i].ClusterIndex[1];

        const BezierCurveNode2& nodeA = m_shapeNodes[indexA].Nodes[clusterIndexA];
        const BezierCurveNode2& nodeB = m_shapeNodes[indexB].Nodes[clusterIndexB];

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

    const glm::mat4 iden = glm::identity<glm::mat4>();

    std::vector<Vertex> dirtyVertices;
    for (unsigned int i = 0; i < m_pathLineCount; ++i)
    {
        const unsigned int indexA = m_pathLines[i].Index[0];
        const unsigned int indexB = m_pathLines[i].Index[1];

        const unsigned char clusterIndexA = m_pathLines[i].ClusterIndex[0];
        const unsigned char clusterIndexB = m_pathLines[i].ClusterIndex[1];

        const PathNode& nodeA = m_pathNodes[indexA].Nodes[clusterIndexA];
        const PathNode& nodeB = m_pathNodes[indexB].Nodes[clusterIndexB];

        glm::vec3 forward = glm::vec3(0, 0, 1);

        for (unsigned int j = 0; j < a_pathSteps; ++j)
        {
            const float lerp = (j + 0U) / (float)a_pathSteps;
            const float nextLerp = (j + 1U) / (float)a_pathSteps;

            const glm::vec3 pos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, lerp);
            const glm::vec3 nextPos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, nextLerp);

            forward = glm::normalize(nextPos - pos);
            
            const float rot = glm::mix(nodeA.Rotation, nodeB.Rotation, lerp);

            glm::vec3 up = glm::vec3(0, 1, 0);
            if (glm::abs(glm::dot(up, forward)) >= 0.95f)
            {
                up = glm::vec3(0, 0, 1);
            }
            // Need to apply rotation around the axis to allow the rotation property to work this should theoretically allow it
            const glm::vec3 right = glm::normalize(glm::angleAxis(rot, forward) * glm::cross(up, forward));
            up = glm::cross(forward, right);

            // Using a rotation matrix because quaternions do not work well with a axis with no rotation
            // Deliberatly mixing axis to correct perspective
            // Forward axis is relative to the next curve point
            const glm::mat3 rotMat = glm::mat3(right, forward, up);

            const glm::vec2 scale = glm::mix(nodeA.Scale, nodeB.Scale, lerp);

            const glm::mat4 mat = glm::scale(iden, glm::vec3(scale.x, 1.0f, scale.y)) * glm::translate(iden, pos) * glm::mat4(rotMat);

            for (unsigned int k = 0; k < shapeVertexCount; ++k)
            {
                const glm::vec3 pos = mat * shapeVertices[k].Position; 
                const glm::vec3 norm = rotMat * shapeVertices[k].Normal;

                dirtyVertices.emplace_back(Vertex{ glm::vec4(pos, 1.0f), norm });
            }
        }

        const glm::vec3 pos = nodeB.Node.GetPosition();
        const float rot = nodeB.Rotation;

        glm::vec3 up = glm::vec3(0, 1, 0);
        if (glm::abs(glm::dot(up, forward)) >= 0.95f)
        {
            up = glm::vec3(0, 0, 1);
        }
 
        const glm::vec3 right = glm::normalize(glm::angleAxis(rot, forward) * glm::cross(up, forward));
        up = glm::cross(forward, right);

        const glm::mat3 rotMat = glm::mat3(right, forward, up);

        const glm::vec2 scale = nodeB.Scale;

        const glm::mat4 mat = glm::scale(iden, glm::vec3(scale.x, 1.0f, scale.y)) * glm::translate(iden, pos) * glm::mat4(rotMat);

        for (unsigned int k = 0; k < shapeVertexCount; ++k)
        {
            const glm::vec3 pos = mat * shapeVertices[k].Position; 
            const glm::vec3 norm = rotMat * shapeVertices[k].Normal;

            dirtyVertices.emplace_back(Vertex{ glm::vec4(pos, 1.0f), norm });
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

            if (glm::dot(diff, diff) <= 0.001f)
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

            if (indexA == indexD)
            {
                continue;
            }

            // Points merged makes a Tri
            if (indexA == indexB || indexA == indexC)
            {
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
            }
        }
    }

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