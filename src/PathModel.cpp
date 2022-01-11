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

void PathModel::SetModelData(const PathNode* a_nodes, unsigned int a_nodeCount, const unsigned int* a_nodeIndices, unsigned int a_nodeIndexCount, 
    const BezierCurveNode2* a_shapeNodes, unsigned int a_shapeNodeCount, const unsigned int* a_shapeIndices, unsigned int a_shapeIndexCount)
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

    m_pathNodes = new PathNode[m_pathNodeCount];
    for (unsigned int i = 0; i < m_pathNodeCount; ++i)
    {
        m_pathNodes[i] = a_nodes[i];
    }

    m_pathIndices = new unsigned int[m_pathIndexCount];
    for (unsigned int i = 0; i < m_pathIndexCount; ++i)
    {
        m_pathIndices[i] = a_nodeIndices[i];
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

    m_shapeNodeCount = a_shapeNodeCount;
    m_shapeIndexCount = a_shapeIndexCount;

    m_shapeNodes = new BezierCurveNode2[m_shapeNodeCount];
    for (unsigned int i = 0; i < m_shapeNodeCount; ++i)
    {
        m_shapeNodes[i] = a_shapeNodes[i];
    }

    m_shapeIndices = new unsigned int[m_shapeIndexCount];
    for (unsigned int i = 0; i < m_shapeIndexCount; ++i)
    {
        m_shapeIndices[i] = a_shapeIndices[i];
    }
}
void PathModel::PassModelData(PathNode* a_nodes, unsigned int a_nodeCount, unsigned int* a_nodeIndices, unsigned int a_nodeIndexCount, 
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
        const PathNode nodeA = m_pathNodes[m_pathIndices[i * 2 + 0U]];
        const PathNode nodeB = m_pathNodes[m_pathIndices[i * 2 + 1U]];

        for (unsigned int j = 0; j <= a_pathSteps; ++j)
        {
            const float lerp = (j + 0U) / (float)a_pathSteps;
            const float nextLerp = (j + 1U) / (float)a_pathSteps;

            const glm::vec3 pos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, lerp);
            const glm::vec3 nextPos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, nextLerp);

            const glm::vec3 forward = glm::normalize(nextPos - pos);
            const float rot = glm::mix(nodeA.Rotation, nodeB.Rotation, lerp);

            glm::vec3 up = glm::vec3(0, 1, 0);
            if (glm::dot(up, forward) >= 0.95f)
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

    tinyxml2::XMLElement* shapeIndicesElement = a_doc->NewElement("Indices");
    shapeElement->InsertEndChild(shapeIndicesElement);

    for (unsigned int i = 0; i < m_shapeIndexCount; ++i)
    {
        tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
        shapeIndicesElement->InsertEndChild(indexElement);

        indexElement->SetText(m_shapeIndices[i]);
    }

    tinyxml2::XMLElement* shapeNodesElement = a_doc->NewElement("Nodes");
    shapeElement->InsertEndChild(shapeNodesElement);

    for (unsigned int i = 0; i < m_shapeNodeCount; ++i)
    {
        const BezierCurveNode2 node = m_shapeNodes[i];

        tinyxml2::XMLElement* shapeNodeElement = a_doc->NewElement("Node");
        shapeNodesElement->InsertEndChild(shapeNodeElement);

        XMLIO::WriteVec2(a_doc, shapeNodeElement, "Position", node.GetPosition());
        XMLIO::WriteVec2(a_doc, shapeNodeElement, "HandlePosition", node.GetHandlePosition());
    }

    tinyxml2::XMLElement* pathElement = a_doc->NewElement("Path");
    rootElement->InsertEndChild(pathElement);
    pathElement->SetAttribute("Steps", m_pathSteps);

    tinyxml2::XMLElement* pathIndicesElement = a_doc->NewElement("Indices");
    pathElement->InsertEndChild(pathIndicesElement);

    for (unsigned int i = 0; i < m_pathIndexCount; ++i)
    {
        tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
        pathIndicesElement->InsertEndChild(indexElement);

        indexElement->SetText(m_pathIndices[i]);
    }

    tinyxml2::XMLElement* pathNodesElement = a_doc->NewElement("Nodes");
    pathElement->InsertEndChild(pathNodesElement);

    for (unsigned int i = 0; i < m_pathNodeCount; ++i)
    {
        const PathNode node = m_pathNodes[i];

        tinyxml2::XMLElement* pathNodeElement = a_doc->NewElement("Node");
        pathNodesElement->InsertEndChild(pathNodeElement);

        XMLIO::WriteVec3(a_doc, pathNodeElement, "Position", node.Node.GetPosition());
        XMLIO::WriteVec3(a_doc, pathNodeElement, "HandlePostion", node.Node.GetHandlePosition());
        
        tinyxml2::XMLElement* rotationElement = a_doc->NewElement("Rotation");
        pathNodeElement->InsertEndChild(rotationElement);

        rotationElement->SetText(node.Rotation);

        XMLIO::WriteVec2(a_doc, pathNodeElement, "Scale", node.Scale, glm::vec2(1));
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
                
                if (strcmp(sStr, "Indices") == 0)
                {
                    std::vector<unsigned int> indices;

                    for (const tinyxml2::XMLElement* iIter = sIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                    {
                        const char* iStr = iIter->Value();

                        if (strcmp(iStr, "Index") == 0)
                        {
                            indices.emplace_back(iIter->UnsignedText());
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(iStr);
                            printf("\n");
                        }
                    }

                    m_shapeIndexCount = indices.size();

                    if (m_shapeIndices != nullptr)
                    {
                        delete[] m_shapeIndices;
                        m_shapeIndices = nullptr;
                    }

                    m_shapeIndices = new unsigned int[m_shapeIndexCount];

                    for (unsigned int i = 0; i < m_shapeIndexCount; ++i)
                    {
                        m_shapeIndices[i] = indices[i];
                    }
                }
                else if (strcmp(sStr, "Nodes") == 0)
                {
                    std::vector<BezierCurveNode2> nodes;

                    for (const tinyxml2::XMLElement* nIter = sIter->FirstChildElement(); nIter != nullptr; nIter = nIter->NextSiblingElement())
                    {
                        const char* nStr = nIter->Value();
                        if (strcmp(nStr, "Node") == 0)
                        {
                            BezierCurveNode2 node;

                            for (const tinyxml2::XMLElement* niIter = nIter->FirstChildElement(); niIter != nullptr; niIter = niIter->NextSiblingElement())
                            {
                                const char* niStr = niIter->Value();

                                if (strcmp(niStr, "Position") == 0)
                                {
                                    node.SetPosition(XMLIO::GetVec2(niIter));
                                }
                                else if (strcmp(niStr, "HandlePosition") == 0)
                                {
                                    node.SetHandlePosition(XMLIO::GetVec2(niIter));
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(niStr);
                                    printf("\n");
                                }
                            }

                            nodes.emplace_back(node);
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

                    m_shapeNodes = new BezierCurveNode2[m_shapeNodeCount];

                    for (unsigned int i = 0; i < m_shapeNodeCount; ++i)
                    {
                        m_shapeNodes[i] = nodes[i];
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

                if (strcmp(pStr, "Indices") == 0)
                {
                    std::vector<unsigned int> indices;

                    for (const tinyxml2::XMLElement* iIter = pIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                    {
                        const char* iStr = iIter->Value();

                        if (strcmp(iStr, "Index") == 0)
                        {
                            indices.emplace_back(iIter->UnsignedText());
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(iStr);
                            printf("\n");
                        }
                    }

                    m_pathIndexCount = indices.size();

                    if (m_pathIndices != nullptr)
                    {
                        delete[] m_pathIndices;
                        m_pathIndices = nullptr;
                    }

                    m_pathIndices = new unsigned int[m_pathIndexCount];

                    for (unsigned int i = 0; i < m_pathIndexCount; ++i)
                    {
                        m_pathIndices[i] = indices[i];
                    }
                }
                else if (strcmp(pStr, "Nodes") == 0)
                {
                    std::vector<PathNode> nodes;

                    for (const tinyxml2::XMLElement* nIter = pIter->FirstChildElement(); nIter != nullptr; nIter = nIter->NextSiblingElement())
                    {
                        const char* nStr = nIter->Value();

                        if (strcmp(nStr, "Node") == 0)
                        {
                            PathNode node;

                            for (const tinyxml2::XMLElement* niIter = nIter->FirstChildElement(); niIter != nullptr; niIter = niIter->NextSiblingElement())
                            {
                                const char* niStr = niIter->Value();

                                if (strcmp(niStr, "Position") == 0)
                                {
                                    node.Node.SetPosition(XMLIO::GetVec3(niIter));
                                }
                                else if (strcmp(niStr, "HandlePostion") == 0)
                                {
                                    node.Node.SetHandlePosition(XMLIO::GetVec3(niIter));
                                }
                                else if (strcmp(niStr, "Rotation") == 0)
                                {
                                    node.Rotation = niIter->FloatText();
                                }
                                else if (strcmp(niStr, "Scale") == 0)
                                {
                                    node.Scale = XMLIO::GetVec2(niIter, glm::vec2(1));
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(niStr);
                                    printf("\n");
                                }
                            }

                            nodes.emplace_back(node);
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

                    m_pathNodes = new PathNode[m_pathNodeCount];

                    for (unsigned int i = 0; i < m_pathNodeCount; ++i)
                    {
                        m_pathNodes[i] = nodes[i];
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