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
void CurveModel::Triangulate()
{
    unsigned int vertexCount;
    unsigned int indexCount;
    Vertex* vertices;
    unsigned int* indices;

    PreTriangulate(&indices, &indexCount, &vertices, &vertexCount);
    PostTriangulate(indices, indexCount, vertices, vertexCount);
}

void CurveModel::PreTriangulate(unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const
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
                        nodes[i] = m_nodes[face.Index[i]].Nodes[face.ClusterIndex[i]];
                    } 

                    const glm::vec3 tpV = nodes[FaceIndex_3Point_AB].GetPosition();

                    // I am not good at maths so I could be wrong but I am sensing a coastline problem here therefore I am just doing
                    // an approximation based on the points instead of the curve
                    int step = m_steps;
                    if (m_stepAdjust)
                    {
                        const float aDist = GetNodeDist(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA]);
                        const float bDist = GetNodeDist(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB]);
                        const float cDist = GetNodeDist(nodes[FaceIndex_3Point_CA], nodes[FaceIndex_3Point_AC]);

                        const float m = glm::max(aDist, glm::max(bDist, cDist));
                        step = (int)glm::ceil(m * m_steps * 0.5f);
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
                        nodes[i] = m_nodes[face.Index[i]].Nodes[face.ClusterIndex[i]];
                    } 

                    int xStep = m_steps;
                    int yStep = m_steps;
                    if (m_stepAdjust)
                    {
                        const float xADist = GetNodeDist(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA]);
                        const float xBDist = GetNodeDist(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC]);
                        const float yADist = GetNodeDist(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA]);
                        const float yBDist = GetNodeDist(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB]);

                        const float mX = glm::max(xADist, xBDist);
                        const float mY = glm::max(yADist, yBDist);
                        
                        xStep = (int)glm::ceil(mX * m_steps * 0.5f);
                        yStep = (int)glm::ceil(mY * m_steps * 0.5f);
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

        if (m_stepAdjust)
        {
            const double cDist = 1.0f / m_steps * 0.75f;
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