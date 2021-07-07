#include "CurveModel.h"

#include "Model.h"

CurveModel::CurveModel()
{
    m_nodes = nullptr;
    m_faces = nullptr;

    m_nodeCount = 0;
    m_faceCount = 0;

    m_displayModel = nullptr;

    m_stepAdjust = false;

    m_steps = 10;
}
CurveModel::~CurveModel()
{
    if (m_nodes != nullptr)
    {
        delete[] m_nodes;
        m_nodes = nullptr;
    }
    if (m_faces != nullptr)
    {
        delete[] m_faces;
        m_faces = nullptr;
    }

    if (m_displayModel != nullptr)
    {
        delete m_displayModel;
        m_displayModel = nullptr;
    }
}

float GetNodeDist(const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB)
{
    const glm::vec3 posA = a_nodeA.GetPosition();
    const glm::vec3 posB = a_nodeB.GetPosition();

    const glm::vec3 handlePosA = a_nodeA.GetHandlePosition();
    const glm::vec3 handlePosB = a_nodeB.GetHandlePosition();

    const glm::vec3 aDiff = posA - handlePosA;
    const glm::vec3 abDiff = handlePosA - handlePosB;
    const glm::vec3 bDiff = posB - handlePosB;

    return glm::length(aDiff) + glm::length(abDiff) + glm::length(bDiff);
}

unsigned int CurveModel::Get3PointFaceIndex(unsigned int a_indexA, unsigned int a_indexB, unsigned int a_indexC) const
{
    const unsigned int arr[] = { a_indexA, a_indexA, a_indexB, a_indexB, a_indexC, a_indexC };

    return Get3PointFaceIndex(arr);
}
unsigned int CurveModel::Get3PointFaceIndex(const unsigned int a_indices[6]) const
{
    for (unsigned int i = 0; i < m_faceCount; ++i)
    {
        const CurveFace face = m_faces[i];

        if (face.FaceMode == FaceMode_3Point)
        {
            bool foundIndex = true;
            for (unsigned int j = 0; j < 6; ++j)
            {
                bool found = false;
                for (unsigned int k = 0; k < 6; ++k)
                {
                    if (face.Index[j] == a_indices[k])
                    {
                        found = true;

                        break;
                    }
                }

                if (!found)
                {
                    foundIndex = false;

                    break;
                }
            }

            if (foundIndex)
            {
                return i;
            }
        }        
    }

    return -1;
}
unsigned int CurveModel::Get4PointFaceIndex(unsigned int a_indexA, unsigned int a_indexB, unsigned int a_indexC, unsigned int a_indexD) const
{
    const unsigned int arr[] = { a_indexA, a_indexA, a_indexB, a_indexB, a_indexC, a_indexC, a_indexD, a_indexD };

    return Get4PointFaceIndex(arr);
}
unsigned int CurveModel::Get4PointFaceIndex(const unsigned int a_indices[8]) const
{
    for (unsigned int i = 0; i < m_faceCount; ++i)
    {
        const CurveFace face = m_faces[i];

        if (face.FaceMode == FaceMode_4Point)
        {
            bool foundIndex = true;
            for (unsigned int j = 0; j < 8; ++j)
            {
                bool found = false;
                for (unsigned int k = 0; k < 8; ++k)
                {
                    if (face.Index[j] == a_indices[k])
                    {
                        found = true;

                        break;
                    }
                }

                if (!found)
                {
                    foundIndex = false;

                    break;
                }
            }

            if (foundIndex)
            {
                return i;
            }
        }        
    }

    return -1;
}

void CurveModel::EmplaceFace(const CurveFace& a_face)
{
    EmplaceFaces(&a_face, 1);
}
void CurveModel::EmplaceFaces(const CurveFace* a_faces, unsigned int a_count)
{
    const unsigned int size = m_faceCount + a_count;

    CurveFace* newFaces = new CurveFace[size];

    if (m_faces != nullptr)
    {
        for (unsigned int i = 0; i < m_faceCount; ++i)
        {
            newFaces[i] = m_faces[i];
        }
    }

    for (unsigned int i = 0; i < a_count; ++i)
    {
        const CurveFace face = a_faces[i];

        newFaces[i + m_faceCount] = a_faces[i];
        switch (face.FaceMode)
        {
        case FaceMode_3Point:
        {
            for (unsigned int j = 0; j < 6; ++j)
            {
                ++m_nodes[face.Index[j]].Nodes[face.ClusterIndex[j]].FaceCount;
            }

            break;
        }
        case FaceMode_4Point:
        {
            for (unsigned int j = 0; j < 8; ++j)
            {
                ++m_nodes[face.Index[j]].Nodes[face.ClusterIndex[j]].FaceCount;
            }

            break;
        }
        }
    }

    if (m_faces != nullptr)
    {
        delete[] m_faces;
        m_faces = nullptr;
    }

    m_faces = newFaces;
    m_faceCount = size;
}

void CurveModel::EmplaceNode(unsigned int a_index, const Node3Cluster& a_node)
{
    const unsigned int size = m_nodeCount + 1;

    Node3Cluster* newNodes = new Node3Cluster[size];

    if (m_nodes != nullptr)
    {
        for (unsigned int i = 0; i < a_index; ++i)
        {
            newNodes[i] = m_nodes[i];
        }
    }
    
    newNodes[a_index] = a_node;

    if (m_nodes != nullptr)
    {
        const unsigned int nextIndex = a_index + 1;

        for (unsigned int i = 0; i < size - nextIndex; ++i)
        {
            newNodes[nextIndex + i] = m_nodes[a_index + i];
        }

        delete[] m_nodes;
    }

    m_nodes = newNodes;
    m_nodeCount = size;
}
void CurveModel::EmplaceNode(const Node3Cluster& a_node)
{
    EmplaceNodes(&a_node, 1);
}
void CurveModel::EmplaceNodes(const Node3Cluster* a_nodes, unsigned int a_count)
{
    const unsigned int size = m_nodeCount + a_count;

    Node3Cluster* newNodes = new Node3Cluster[size];

    if (m_nodes != nullptr)
    {
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            newNodes[i] = m_nodes[i];
        }
    }

    for (unsigned int i = 0; i < a_count; ++i)
    {
        newNodes[i + m_nodeCount] = a_nodes[i];
    }

    if (m_nodes != nullptr)
    {
        delete[] m_nodes;
        m_nodes = nullptr;
    }

    m_nodes = newNodes;
    m_nodeCount = size;
}

void CurveModel::DestroyFace(unsigned int a_index)
{
    DestroyFaces(a_index, a_index + 1);
}
void CurveModel::DestroyFaces(unsigned int a_start, unsigned int a_end)
{
    const unsigned int count = a_end - a_start;
    const unsigned int size = m_faceCount - count;
    const unsigned int endCount = m_faceCount - a_end;

    CurveFace* newFaces = new CurveFace[size];

    if (m_faces != nullptr)
    {
        for (unsigned int i = 0; i < a_start; ++i)
        {
            newFaces[i] = m_faces[i];
        }

        for (unsigned int i = 0; i < endCount; ++i)
        {
            newFaces[i + a_start] = m_faces[i + a_end];
        }

        for (unsigned int i = a_start; i < a_end; ++i)
        {
            const CurveFace face = m_faces[i];

            switch (face.FaceMode)
            {
            case FaceMode_3Point:
            {
                for (int j = 0; j < 6; ++j)
                {
                    --m_nodes[face.Index[j]].Nodes[face.Index[j]].FaceCount;
                }

                break;
            }
            case FaceMode_4Point:
            {
                for (int j = 0; j < 8; ++j)
                {
                    --m_nodes[face.Index[j]].Nodes[face.Index[j]].FaceCount;
                }

                break;
            }
            }

            
        }

        delete[] m_faces;
        m_faces = nullptr;
    }

    m_faces = newFaces;
    m_faceCount = size;
}

void CurveModel::DestroyNode(unsigned int a_index)
{
    DestroyNodes(a_index, a_index + 1);
}
void CurveModel::DestroyNodes(unsigned int a_start, unsigned int a_end)
{
    const unsigned int count = a_end - a_start;
    const unsigned int size = m_nodeCount - count;
    const unsigned int endCount = m_nodeCount - a_end;

    Node3Cluster* newNodes = new Node3Cluster[size];

    if (m_nodes != nullptr)
    {
        for (unsigned int i = 0; i < a_start; ++i)
        {
            newNodes[i] = m_nodes[i];
        }

        for (unsigned int i = 0; i < endCount; ++i)
        {
            newNodes[i + a_start] = m_nodes[i + a_end];
        }

        delete[] m_nodes;
        m_nodes = nullptr;
    }

    m_nodes = newNodes;
    m_nodeCount = size;
}

void CurveModel::SetModelData(Node3Cluster* a_nodes, unsigned int a_nodeCount, CurveFace* a_faces, unsigned int a_faceCount)
{
    if (m_nodes != nullptr)
    {
        delete[] m_nodes;
        m_nodes = nullptr;
    }
    if (m_faces != nullptr)
    {
        delete[] m_faces;
        m_faces = nullptr;
    }

    m_nodes = a_nodes;
    m_faces = a_faces;

    m_nodeCount = a_nodeCount;
    m_faceCount = a_faceCount;
}
void CurveModel::PassModelData(Node3Cluster* a_nodes, unsigned int a_nodeCount, CurveFace* a_faces, unsigned int a_faceCount)
{
    m_nodes = a_nodes;
    m_faces = a_faces;

    m_nodeCount = a_nodeCount;
    m_faceCount = a_faceCount;
}
void CurveModel::Triangulate()
{
    unsigned int vertexCount;
    unsigned int indexCount;
    Vertex* vertices;
    unsigned int* indices;

    PreTriangulate(&indices, &indexCount, &vertices, &vertexCount);
    PostTriangulate(indices, indexCount, vertices, vertexCount);
}

void CurveModel::GetModelData(bool a_smartStep, int a_steps, unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const
{
    *a_vertices = nullptr;
    *a_indices = nullptr;
    *a_indexCount = 0;
    *a_vertexCount = 0;

    if (m_faceCount > 0)
    {
        std::vector<Vertex> dirtyVertices;

        // All of this is just pulled out of nothing and has issues needs to be verified and fixed
        // Probably need to poke someone good at maths
        for (int i = 0; i < m_faceCount; ++i)
        {
            const CurveFace face = m_faces[i];

            switch (face.FaceMode)
            {
                case FaceMode_3Point:
                {
                    BezierCurveNode3 nodes[6];

                    for (int i = 0; i < 6; ++i)
                    {
                        nodes[i] = m_nodes[face.Index[i]].Nodes[face.ClusterIndex[i]].Node;
                    } 

                    const glm::vec3 tpV = nodes[FaceIndex_3Point_AB].GetPosition();

                    // I am not good at maths so I could be wrong but I am sensing a coastline problem here therefore I am just doing
                    // an approximation based on the points instead of the curve
                    int step = a_steps;
                    if (a_smartStep)
                    {
                        const float aDist = GetNodeDist(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA]);
                        const float bDist = GetNodeDist(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB]);
                        const float cDist = GetNodeDist(nodes[FaceIndex_3Point_CA], nodes[FaceIndex_3Point_AC]);

                        const float m = glm::max(aDist, glm::max(bDist, cDist));
                        step = (int)glm::ceil(m * a_steps * 0.5f);
                    }

                    // Still not 100% and I suspect I went down the wrong path trying to triangulate it 
                    // Should be close enough for demo purposes however will need to fix down the line
                    for (int i = 0; i < step; ++i)
                    {
                        const float iStep = (float)i / step;
                        const float nIStep = (float)(i + 1) / step;
                        const float bIStep = (float)(i + 2) / step;

                        const glm::vec3 tL = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], 2.0f, iStep);
                        const glm::vec3 tR = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], 2.0f, iStep);
                        const glm::vec3 mL = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], 2.0f, nIStep);
                        const glm::vec3 mR = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], 2.0f, nIStep);
                        const glm::vec3 bL = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], 2.0f, bIStep);
                        const glm::vec3 bR = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], 2.0f, bIStep);

                        for (int j = 0; j <= i; ++j)
                        {
                            float aS = j / (float)i;
                            if (j <= 0 || i <= 0)
                            {
                                aS = 0;
                            }
                            
                            const float aSMA = j / (float)(i + 1);
                            const float aSMB = (j + 1) / (float)(i + 1);
                            
                            const glm::vec3 tB = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], 2.0f, aS);
                            const glm::vec3 mLB = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], 2.0f, aSMA);
                            const glm::vec3 mRB = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], 2.0f, aSMB);
                            
                            const glm::vec3 t = glm::mix(tL, tR, aS);
                            const glm::vec3 mA = glm::mix(mL, mR, aSMA);
                            const glm::vec3 mB = glm::mix(mL, mR, aSMB); 

                            const glm::vec3 tS = glm::mix(tpV, tB, iStep);
                            const glm::vec3 mSA = glm::mix(tpV, mLB, nIStep);
                            const glm::vec3 mSB = glm::mix(tpV, mRB, nIStep);

                            const glm::vec3 tF = (t + tS) * 0.5f;
                            const glm::vec3 mLF = (mA + mSA) * 0.5f;
                            const glm::vec3 mRF = (mB + mSB) * 0.5f;

                            glm::vec3 v1 = mLF - tF;
                            glm::vec3 v2 = mRF - tF;

                            glm::vec3 normal = glm::cross(v2, v1);

                            dirtyVertices.emplace_back(Vertex{ { tF, 1.0f }, normal, { 0.0f, 0.0f }});
                            dirtyVertices.emplace_back(Vertex{ { mLF, 1.0f }, normal, { 0.0f, 0.0f }});
                            dirtyVertices.emplace_back(Vertex{ { mRF, 1.0f }, normal, { 0.0f, 0.0f }});

                            if (i < step - 1)
                            {
                                const float aSL = (j + 1) / (float)(i + 2);

                                const glm::vec3 bB = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], 2.0f, aSL);

                                const glm::vec3 b = glm::mix(bL, bR, aSL);

                                const glm::vec3 bS = glm::mix(tpV, bB, bIStep);

                                const glm::vec3 bF = (b + bS) * 0.5f;

                                v1 = mLF - bF;
                                v2 = mRF - bF;

                                normal = glm::cross(v1, v2);

                                dirtyVertices.emplace_back(Vertex{ { bF, 1.0f }, normal, { 0.0f, 0.0f }});
                                dirtyVertices.emplace_back(Vertex{ { mRF, 1.0f }, normal, { 0.0f, 0.0f }});
                                dirtyVertices.emplace_back(Vertex{ { mLF, 1.0f }, normal, { 0.0f, 0.0f }});
                            }
                        }
                    }

                    break; 
                }
                case FaceMode_4Point:
                {
                    BezierCurveNode3 nodes[8];

                    for (int i = 0; i < 8; ++i)
                    {
                        nodes[i] = m_nodes[face.Index[i]].Nodes[face.ClusterIndex[i]].Node;
                    } 

                    int xStep = a_steps;
                    int yStep = a_steps;
                    if (a_smartStep)
                    {
                        const float xADist = GetNodeDist(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA]);
                        const float xBDist = GetNodeDist(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC]);
                        const float yADist = GetNodeDist(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA]);
                        const float yBDist = GetNodeDist(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB]);

                        const float mX = glm::max(xADist, xBDist);
                        const float mY = glm::max(yADist, yBDist);
                        
                        xStep = (int)glm::ceil(mX * a_steps * 0.5f);
                        yStep = (int)glm::ceil(mY * a_steps * 0.5f);
                    }

                    for (int i = 0; i < xStep; ++i)
                    {
                        const float iStep = (float)i / xStep;
                        const float nIStep = (float)(i + 1) / xStep;

                        const glm::vec3 pointABLeft = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], 2.0f, iStep);
                        const glm::vec3 pointABRight = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], 2.0f, nIStep);
                        const glm::vec3 pointCDLeft = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], 2.0f, iStep);
                        const glm::vec3 pointCDRight = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], 2.0f, nIStep);

                        for (int j = 0; j < yStep; ++j)
                        {
                            const float jStep = (float)j / yStep;
                            const float nJStep = (float)(j + 1) / yStep;

                            const glm::vec3 pointACLeft = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], 2.0f, jStep);
                            const glm::vec3 pointACRight = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], 2.0f, nJStep);
                            const glm::vec3 pointDBLeft = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], 2.0f, jStep);
                            const glm::vec3 pointDBRight = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], 2.0f, nJStep);

                            const glm::vec3 LLA = glm::mix(pointABLeft, pointCDLeft, jStep);
                            const glm::vec3 LHA = glm::mix(pointABLeft, pointCDLeft, nJStep);
                            const glm::vec3 HLA = glm::mix(pointABRight, pointCDRight, jStep);
                            const glm::vec3 HHA = glm::mix(pointABRight, pointCDRight, nJStep);

                            const glm::vec3 LLB = glm::mix(pointACLeft, pointDBLeft, iStep);
                            const glm::vec3 LHB = glm::mix(pointACLeft, pointDBLeft, nIStep);
                            const glm::vec3 HLB = glm::mix(pointACRight, pointDBRight, iStep);
                            const glm::vec3 HHB = glm::mix(pointACRight, pointDBRight, nIStep);

                            const glm::vec3 posA = (LLA + LLB) * 0.5f;
                            const glm::vec3 posB = (HLA + LHB) * 0.5f;
                            const glm::vec3 posC = (LHA + HLB) * 0.5f;
                            const glm::vec3 posD = (HHA + HHB) * 0.5f;

                            glm::vec3 v1 = posB - posA;
                            glm::vec3 v2 = posC - posA;

                            glm::vec3 normal = glm::cross(v2, v1);

                            dirtyVertices.emplace_back(Vertex{ { posA, 1.0f }, normal, { 0.0f, 0.0f }});
                            dirtyVertices.emplace_back(Vertex{ { posB, 1.0f }, normal, { 0.0f, 0.0f }});
                            dirtyVertices.emplace_back(Vertex{ { posC, 1.0f }, normal, { 0.0f, 0.0f }});

                            dirtyVertices.emplace_back(Vertex{ { posB, 1.0f }, normal, { 0.0f, 0.0f }});
                            dirtyVertices.emplace_back(Vertex{ { posD, 1.0f }, normal, { 0.0f, 0.0f }});
                            dirtyVertices.emplace_back(Vertex{ { posC, 1.0f }, normal, { 0.0f, 0.0f }});
                        }
                    }

                    break;
                }
            }
        }

        *a_indexCount = dirtyVertices.size();

        // Extremely unlikely that there is going to be that many vertices but means I do not have to allocate more if I make it that big
        *a_vertices = new Vertex[*a_indexCount];
        *a_indices = new unsigned int[*a_indexCount];

        unsigned int vertexIndex = 0;

        if (a_smartStep)
        {
            const double cDist = 1.0f / a_steps * 0.75f;
            const double cDSqr = cDist * cDist;

            for (unsigned int i = 0; i < *a_indexCount; ++i)
            {
                const Vertex vert = dirtyVertices[i];
    
                bool found = false;
    
                for (unsigned int j = 0; j < vertexIndex; ++j)
                {
                    const Vertex cVert = (*a_vertices)[j];

                    const glm::vec3 diff = vert.Position - cVert.Position;

                    if (glm::dot(diff, diff) < cDSqr)
                    {
                        found = true;
                        
                        (*a_vertices)[j].Normal += vert.Normal;
                        (*a_indices)[i] = j;
    
                        break;
                    }
                }
    
                if (!found)
                {
                    (*a_vertices)[vertexIndex] = vert;
                    (*a_indices)[i] = vertexIndex++;
                }
            }
        }
        else
        {
            for (unsigned int i = 0; i < *a_indexCount; ++i)
            {
                const Vertex vert = dirtyVertices[i];

                bool found = false;

                for (unsigned int j = 0; j < vertexIndex; ++j)
                {
                    if ((*a_vertices)[j].Position == vert.Position)
                    {
                        found = true;

                        (*a_vertices)[j].Normal += vert.Normal;
                        (*a_indices)[i] = j;

                        break;
                    }
                }

                if (!found)
                {
                    (*a_vertices)[vertexIndex] = vert;
                    (*a_indices)[i] = vertexIndex++;
                }
            }
        }
        
        *a_vertexCount = vertexIndex;

        for (unsigned int i = 0; i < *a_vertexCount; ++i)
        {
            (*a_vertices)[i].Normal = glm::normalize((*a_vertices)[i].Normal);
        }
    }
}

void CurveModel::PreTriangulate(unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const
{
    GetModelData(m_stepAdjust, m_steps, a_indices, a_indexCount, a_vertices, a_vertexCount);
}
void CurveModel::PostTriangulate(unsigned int* a_indices, unsigned int a_indexCount, Vertex* a_vertices, unsigned int a_vertexCount)
{
    if (m_displayModel != nullptr)
    {
        delete m_displayModel;
        m_displayModel = nullptr;
    }

    if (a_vertexCount != 0 && a_indexCount != 0)
    {
        m_displayModel = new Model(a_vertices, a_indices, a_vertexCount, a_indexCount);
    }

    delete[] a_vertices;
    delete[] a_indices;
}

void CurveModel::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent) const
{
    tinyxml2::XMLElement* curveModelElement = a_doc->NewElement("CurveModel");
    a_parent->InsertEndChild(curveModelElement);

    curveModelElement->SetAttribute("StepAdjust", m_stepAdjust);
    curveModelElement->SetAttribute("Steps", m_steps);

    tinyxml2::XMLElement* facesElement = a_doc->NewElement("Faces");
    curveModelElement->InsertEndChild(facesElement);

    for (unsigned int i = 0; i < m_faceCount; ++i)
    {
        const CurveFace face = m_faces[i];

        tinyxml2::XMLElement* fElement = a_doc->NewElement("Face");
        facesElement->InsertEndChild(fElement);

        fElement->SetAttribute("FaceMode", (int)face.FaceMode);

        switch (face.FaceMode)
        {
        case FaceMode_3Point:
        {
            for (int i = 0; i < 6; ++i)
            {
                tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
                fElement->InsertEndChild(indexElement);
                indexElement->SetText(face.Index[i]);

                tinyxml2::XMLElement* clusterIndexElement = a_doc->NewElement("ClusterIndex");
                fElement->InsertEndChild(clusterIndexElement);
                clusterIndexElement->SetText(face.ClusterIndex[i]);
            }

            break;
        }
        case FaceMode_4Point:
        {
            for (int i = 0; i < 8; ++i)
            {
                tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
                fElement->InsertEndChild(indexElement);
                indexElement->SetText(face.Index[i]);

                tinyxml2::XMLElement* clusterIndexElement = a_doc->NewElement("ClusterIndex");
                fElement->InsertEndChild(clusterIndexElement);
                clusterIndexElement->SetText(face.ClusterIndex[i]);
            }

            break;
        }
        }
    }

    tinyxml2::XMLElement* nodesElement = a_doc->NewElement("Nodes");
    curveModelElement->InsertEndChild(nodesElement);

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const Node3Cluster node = m_nodes[i];

        tinyxml2::XMLElement* nElement = a_doc->NewElement("Node");
        nodesElement->InsertEndChild(nElement);

        const std::vector<NodeGroup> nodes = node.Nodes;
        const int size = nodes.size();

        if (size > 0)
        {
            const glm::vec3 pos = nodes[0].Node.GetPosition();

            tinyxml2::XMLElement* positionElement = a_doc->NewElement("Position");
            nElement->InsertEndChild(positionElement);

            tinyxml2::XMLElement* pXElement = a_doc->NewElement("X");
            positionElement->InsertEndChild(pXElement);
            pXElement->SetText(pos.x);
            tinyxml2::XMLElement* pYElement = a_doc->NewElement("Y");
            positionElement->InsertEndChild(pYElement);
            pYElement->SetText(pos.y);
            tinyxml2::XMLElement* pZElement = a_doc->NewElement("Z");
            positionElement->InsertEndChild(pZElement);
            pZElement->SetText(pos.z);

            for (int j = 0; j < size; ++j)
            {
                const NodeGroup g = nodes[j];

                tinyxml2::XMLElement* cNodeElement = a_doc->NewElement("ClusterNode");
                nElement->InsertEndChild(cNodeElement);

                const glm::vec3 hPos = g.Node.GetHandlePosition();

                tinyxml2::XMLElement* hPositionElement = a_doc->NewElement("HandlePosition");
                cNodeElement->InsertEndChild(hPositionElement);

                tinyxml2::XMLElement* pHXElement = a_doc->NewElement("X");
                hPositionElement->InsertEndChild(pHXElement);
                pHXElement->SetText(hPos.x);
                tinyxml2::XMLElement* pHYElement = a_doc->NewElement("Y");
                hPositionElement->InsertEndChild(pHYElement);
                pHYElement->SetText(hPos.y);
                tinyxml2::XMLElement* pHZElement = a_doc->NewElement("Z");
                hPositionElement->InsertEndChild(pHZElement);
                pHZElement->SetText(hPos.z);
            }
        }
    }
}


void CurveModel::WriteOBJ(std::ofstream* a_file, bool a_stepAdjust, int a_steps)
{
    Vertex* vertices;
    unsigned int* indices;

    unsigned int indexCount;
    unsigned int vertexCount;

    GetModelData(a_stepAdjust, a_steps, &indices, &indexCount, &vertices, &vertexCount);

    a_file->write("\n", 1);

    const char* vertexPosComment = "# Vertex Data \n";
    a_file->write(vertexPosComment, strlen(vertexPosComment));

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        const Vertex vert = vertices[i];

        a_file->write("v", 1);
        for (int j = 0; j < 4; ++j)
        {
            const std::string str = " " + std::to_string(vert.Position[j]);
            a_file->write(str.c_str(), str.length());
        }
        a_file->write("\n", 1);

        a_file->write("vn", 2);
        for (int j = 0; j < 3; ++j)
        {
            const std::string str = " " + std::to_string(-vert.Normal[j]);
            a_file->write(str.c_str(), str.length());
        }    
        a_file->write("\n", 1);
    }

    a_file->write("\n", 1);

    const char* facesComment = "# Faces \n";
    a_file->write(facesComment, strlen(facesComment));

    for (unsigned int i = 0; i < indexCount; i += 3)
    {
        a_file->write("f", 1);
        
        for (int j = 0; j < 3; ++j)
        {
            const std::string strVal = std::to_string(indices[i + j] + 1);
            const std::string str = " " + strVal + "//" + strVal;
            a_file->write(str.c_str(), str.length());
        }

        a_file->write(" \n", 2);    
    }
}