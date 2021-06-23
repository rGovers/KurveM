#include "CurveModel.h"

#include "Model.h"

CurveModel::CurveModel()
{
    m_nodes = nullptr;
    m_faces = nullptr;

    m_nodeCount = 0;
    m_faceCount = 0;

    m_displayModel = nullptr;

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
    if (m_displayModel != nullptr)
    {
        delete m_displayModel;
        m_displayModel = nullptr;
    }

    if (m_faceCount > 0)
    {
        std::vector<Vertex> dirtyVertices;

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

                    const glm::vec3 tpV = nodes[FaceIndex_3Point_AB].Position();

                    // Still not 100% and I suspect I went down the wrong path trying to triangulate it 
                    // Should be close enough for demo purposes however will need to fix down the line
                    for (int i = 0; i < m_steps; ++i)
                    {
                        const float iStep = (float)i / m_steps;
                        const float nIStep = (float)(i + 1) / m_steps;
                        const float bIStep = (float)(i + 2) / m_steps;

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

                            if (i < m_steps - 1)
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

                    for (int i = 0; i < m_steps; ++i)
                    {
                        const float iStep = (float)i / m_steps;
                        const float nIStep = (float)(i + 1) / m_steps;

                        const glm::vec3 pointABLeft = BezierCurveNode3::GetPoint(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], iStep);
                        const glm::vec3 pointABRight = BezierCurveNode3::GetPoint(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], nIStep);
                        const glm::vec3 pointCDLeft = BezierCurveNode3::GetPoint(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], iStep);
                        const glm::vec3 pointCDRight = BezierCurveNode3::GetPoint(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], nIStep);

                        for (int j = 0; j < m_steps; ++j)
                        {
                            const float jStep = (float)j / m_steps;
                            const float nJStep = (float)(j + 1) / m_steps;

                            const glm::vec3 pointACLeft = BezierCurveNode3::GetPoint(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], jStep);
                            const glm::vec3 pointACRight = BezierCurveNode3::GetPoint(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], nJStep);
                            const glm::vec3 pointDBLeft = BezierCurveNode3::GetPoint(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], jStep);
                            const glm::vec3 pointDBRight = BezierCurveNode3::GetPoint(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], nJStep);

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

        const unsigned int indexCount = dirtyVertices.size();

        // Extremely unlikely that there is going to be that many vertices but means I do not have to allocate more if I make it that big
        Vertex* vertices = new Vertex[indexCount];
        unsigned int* indices = new unsigned int[indexCount];

        unsigned int vertexIndex = 0;

        for (unsigned int i = 0; i < indexCount; ++i)
        {
            const Vertex vert = dirtyVertices[i];

            bool found = false;

            for (unsigned int j = 0; j < vertexIndex; ++j)
            {
                if (vertices[j].Position == vert.Position)
                {
                    found = true;

                    vertices[j].Normal += vert.Normal;
                    indices[i] = j;

                    break;
                }
            }

            if (!found)
            {
                vertices[vertexIndex] = vert;
                indices[i] = vertexIndex++;
            }
        }

        for (unsigned int i = 0; i < vertexIndex; ++i)
        {
            vertices[i].Normal = glm::normalize(vertices[i].Normal);
        }

        m_displayModel = new Model(vertices, indices, vertexIndex, indexCount);

        delete[] vertices;
        delete[] indices;
    }
}