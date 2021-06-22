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

                    // Was feeling cheeky reusing quad maths but ended up slightly too squared to work properly
                    // for (int i = 0; i < m_steps; ++i)
                    // {
                    //     const float iStep = (float)i / m_steps;
                    //     const float nIStep = (float)(i + 1) / m_steps;

                    //     const glm::vec3 pointABLeftLow = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], iStep);
                    //     const glm::vec3 pointABRightLow = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], nIStep);
                    //     const glm::vec3 pointABLeftHigh = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_CB], nodes[FaceIndex_3Point_BC], iStep * 0.5f);
                    //     const glm::vec3 pointABRightHigh = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_CB], nodes[FaceIndex_3Point_BC], nIStep * 0.5f);

                    //     for (int j = 0; j < m_steps; ++j)
                    //     {
                    //         const float jStep = (float)j / m_steps;
                    //         const float nJStep = (float)(j + 1) / m_steps;

                    //         const glm::vec3 pointACLeftLow = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], jStep);
                    //         const glm::vec3 pointACRightLow = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], nJStep);
                    //         const glm::vec3 pointACLeftHigh = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], jStep * 0.5f);
                    //         const glm::vec3 pointACRightHigh = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], nJStep * 0.5f);

                    //         const glm::vec3 LLA = glm::mix(pointABLeftLow, pointABLeftHigh, jStep);
                    //         const glm::vec3 LHA = glm::mix(pointABLeftLow, pointABLeftHigh, nJStep);
                    //         const glm::vec3 HLA = glm::mix(pointABRightLow, pointABRightHigh, jStep);
                    //         const glm::vec3 HHA = glm::mix(pointABRightLow, pointABRightHigh, nJStep);

                    //         const glm::vec3 LLB = glm::mix(pointACLeftLow, pointACLeftHigh, iStep);
                    //         const glm::vec3 LHB = glm::mix(pointACLeftLow, pointACLeftHigh, nIStep);
                    //         const glm::vec3 HLB = glm::mix(pointACRightLow, pointACRightHigh, iStep);
                    //         const glm::vec3 HHB = glm::mix(pointACRightLow, pointACRightHigh, nIStep);
                            
                    //         const glm::vec3 posA = (LLA + LLB) * 0.5f;
                    //         const glm::vec3 posB = (HLA + LHB) * 0.5f;
                    //         const glm::vec3 posC = (LHA + HLB) * 0.5f;
                    //         const glm::vec3 posD = (HHA + HHB) * 0.5f;

                    //         glm::vec3 v1 = posB - posA;
                    //         glm::vec3 v2 = posC - posA;

                    //         glm::vec3 normal = glm::cross(v2, v1);

                    //         dirtyVertices.emplace_back(Vertex{ { posA, 1.0f }, normal, { 0.0f, 0.0f }});
                    //         dirtyVertices.emplace_back(Vertex{ { posB, 1.0f }, normal, { 0.0f, 0.0f }});
                    //         dirtyVertices.emplace_back(Vertex{ { posC, 1.0f }, normal, { 0.0f, 0.0f }});

                    //         // if (i < m_steps - 1 || j < m_steps - 1)
                    //         // {
                    //         //     glm::vec3 v1 = posB - posD;
                    //         //     glm::vec3 v2 = posC - posD;

                    //         //     glm::vec3 normal = glm::cross(v2, v1);

                    //         //     dirtyVertices.emplace_back(Vertex{ { posB, 1.0f }, normal, { 0.0f, 0.0f }});
                    //         //     dirtyVertices.emplace_back(Vertex{ { posD, 1.0f }, normal, { 0.0f, 0.0f }});
                    //         //     dirtyVertices.emplace_back(Vertex{ { posC, 1.0f }, normal, { 0.0f, 0.0f }});
                    //         // }
                    //     }
                    // }

                    // Still not 100% and I suspect I went down the wrong path trying to triangulate it 
                    // Should be close enough for demo purposes however will need to fix down the line
                    for (int i = 0; i < m_steps; ++i)
                    {
                        const float iStep = (float)i / m_steps;
                        const float nIStep = (float)(i + 1) / m_steps;
                        const float bIStep = (float)(i + 2) / m_steps;

                        const glm::vec3 tL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], iStep);
                        const glm::vec3 tR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], iStep);
                        const glm::vec3 mL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], nIStep);
                        const glm::vec3 mR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], nIStep);
                        const glm::vec3 bL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], bIStep);
                        const glm::vec3 bR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], bIStep);

                        for (int j = 0; j <= i; ++j)
                        {
                            float aS = j / (float)i;
                            if (j <= 0 || i <= 0)
                            {
                                aS = 0;
                            }
                            
                            const float aSMA = j / (float)(i + 1);
                            const float aSMB = (j + 1) / (float)(i + 1);

                            const glm::vec2 tVL = glm::vec2(aS, 1.0f - iStep);
                            glm::vec2 nTVL = glm::vec2(0.0f);
                            if (glm::abs(tVL.x) + glm::abs(tVL.y) > 0.0f)
                            {
                                nTVL = glm::normalize(tVL);
                            }
                            const glm::vec2 tVR = glm::vec2(1.0f - aS, 1.0f - iStep);
                            glm::vec2 nTVR = glm::vec2(0.0f);
                            if (glm::abs(tVR.x) + glm::abs(tVR.y) > 0.0f)
                            {
                                nTVR = glm::normalize(tVR);
                            }

                            const float dA = glm::max(glm::dot(glm::vec2(1.0, 0.0f), nTVL), 0.0f);
                            const float dB = glm::max(glm::dot(glm::vec2(1.0, 0.0f), nTVR), 0.0f);

                            const glm::vec2 mVAL = glm::vec2(aSMA, 1.0f - nIStep);
                            glm::vec2 nMVAL = glm::vec2(0.0f);
                            if (glm::abs(mVAL.x) + glm::abs(mVAL.y) > 0.0f)
                            {
                                nMVAL = glm::normalize(mVAL);
                            }
                            const glm::vec2 mVAR = glm::vec2(1.0f - aSMA, 1.0f - nIStep);
                            glm::vec2 nMVAR = glm::vec2(0.0f);
                            if (glm::abs(mVAR.x) + glm::abs(mVAR.y) > 0.0f)
                            {
                                nMVAR = glm::normalize(mVAR);
                            }
                            const glm::vec2 mVBL = glm::vec2(aSMB, 1.0f - nIStep);
                            glm::vec2 nMVBL = glm::vec2(0.0f);
                            if (glm::abs(mVBL.x) + glm::abs(mVBL.y) > 0.0f)
                            {
                                nMVBL = glm::normalize(mVBL);
                            }
                            const glm::vec2 mVBR = glm::vec2(1.0f - aSMB, 1.0f - nIStep);
                            glm::vec2 nMVBR = glm::vec2(0.0f);
                            if (glm::abs(mVBR.x) + glm::abs(mVBR.y) > 0.0f)
                            {
                                nMVBR = glm::normalize(mVBR);
                            }

                            const float dMLA = glm::max(glm::dot(glm::vec2(1.0, 0.0f), nMVAL), 0.0f);
                            const float dMLB = glm::max(glm::dot(glm::vec2(1.0, 0.0f), nMVAR), 0.0f);
                            const float dMRA = glm::max(glm::dot(glm::vec2(1.0, 0.0f), nMVBL), 0.0f);
                            const float dMRB = glm::max(glm::dot(glm::vec2(1.0, 0.0f), nMVBR), 0.0f);

                            const float s = (dA * dA) * (dB * dB);
                            const float sBL = (dMLA * dMLA) * (dMLB * dMLB);
                            const float sBR = (dMRA * dMRA) * (dMRB * dMRB);

                            const float bSA = j / (float)(i + 1);
                            const float bSB = (j + 1) / (float)(i + 1); 
                            
                            const glm::vec3 tB = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], aS);
                            const glm::vec3 mLB = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], bSA);
                            const glm::vec3 mRB = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], bSB);
                            
                            const glm::vec3 t = glm::mix(tL, tR, aS);
                            const glm::vec3 mA = glm::mix(mL, mR, bSA);
                            const glm::vec3 mB = glm::mix(mL, mR, bSB); 

                            const glm::vec3 tF = glm::mix(t, tB, s);
                            const glm::vec3 mLF = glm::mix(mA, mLB, sBL);
                            const glm::vec3 mRF = glm::mix(mB, mRB, sBR);

                            glm::vec3 v1 = mLF - tF;
                            glm::vec3 v2 = mRF - tF;

                            glm::vec3 normal = glm::cross(v2, v1);

                            dirtyVertices.emplace_back(Vertex{ { tF, 1.0f }, normal, { 0.0f, 0.0f }});
                            dirtyVertices.emplace_back(Vertex{ { mLF, 1.0f }, normal, { 0.0f, 0.0f }});
                            dirtyVertices.emplace_back(Vertex{ { mRF, 1.0f }, normal, { 0.0f, 0.0f }});

                            if (i < m_steps - 1)
                            {
                                const float bS = (j + 1) / (float)(i + 2);

                                const glm::vec2 bVL = glm::vec2(bS, 1.0f - bIStep);
                                glm::vec2 nBVL = glm::vec2(0.0f);
                                if (glm::abs(bVL.x) + glm::abs(bVL.y) > 0.0f)
                                {
                                    nBVL = glm::normalize(bVL);
                                }
                                const glm::vec2 bVR = glm::vec2(1.0f - bS, 1.0f - bIStep);
                                glm::vec2 nBVR = glm::vec2(0.0f);
                                if (glm::abs(bVR.x) + glm::abs(bVR.y) > 0.0f)
                                {
                                    nBVR = glm::normalize(bVR);
                                }

                                const float dBA = glm::max(glm::dot(glm::vec2(1.0, 0.0f), nBVL), 0.0f);
                                const float dBB = glm::max(glm::dot(glm::vec2(1.0, 0.0f), nBVR), 0.0f);

                                const float sB = (dBA * dBA) * (dBB * dBB);

                                const glm::vec3 bB = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], bS);

                                const glm::vec3 b = glm::mix(bL, bR, bS);
                                const glm::vec3 bF = glm::mix(b, bB, sB);

                                v1 = mLF - bF;
                                v2 = mRF - bF;

                                normal = glm::cross(v2, v1);

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