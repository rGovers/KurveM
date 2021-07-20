#include "CurveModel.h"

#include "Model.h"
#include "Object.h"
#include "Workspace.h"
#include "XMLIO.h"

#include <string>
#include <unordered_map>

CurveModel::CurveModel(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_nodes = nullptr;
    m_faces = nullptr;

    m_nodeCount = 0;
    m_faceCount = 0;

    m_displayModel = nullptr;

    m_stepAdjust = false;

    m_armature = -1;

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

void CurveModel::SetArmature(long long a_id)
{
    const Object* obj = m_workspace->GetObject(a_id);

    SetArmature(obj);
}
void CurveModel::SetArmature(const Object* a_armature)
{
    m_armature = -1;

    if (a_armature != nullptr)
    {
        const e_ObjectType type = a_armature->GetObjectType();

        if (type == ObjectType_Armature)
        {
            m_armature = a_armature->GetID();
        }
    }
}
Object* CurveModel::GetArmature() const
{
    return m_workspace->GetObject(m_armature);
}

void GetArmNodes(std::list<Object*>* a_list, Object* a_object)
{
    if (a_object != nullptr && a_object->GetObjectType() == ObjectType_ArmatureNode)
    {
        a_list->emplace_back(a_object);

        const std::list<Object*> children = a_object->GetChildren();
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            GetArmNodes(a_list, *iter);
        }
    }
}

std::list<Object*> CurveModel::GetArmatureNodes() const
{
    std::list<Object*> nodes;

    const Object* obj = m_workspace->GetObject(m_armature);
    if (obj != nullptr && obj->GetObjectType() == ObjectType_Armature)
    {
        const std::list<Object*> children = obj->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            GetArmNodes(&nodes, *iter);
        }
    }

    return nodes;
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

void GetBoneData(const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB, float a_lerp, glm::vec4* a_weights, glm::vec4* a_bones, unsigned int a_boneCount, const std::unordered_map<long long, unsigned int>& a_idMap)
{
    unsigned int count;
    BoneCluster* bones = BezierCurveNode3::GetBonesLerp(a_nodeA, a_nodeB, a_lerp, &count);

    *a_bones = glm::vec4(-1);
    *a_weights = glm::vec4(0);

    for (int i = 0; i < count; ++i)
    {
        const BoneCluster bone = bones[i];

        int index = 0;
        float min = std::numeric_limits<float>::infinity();
        for (int j = 0; j < 4; ++j)
        {
            const float val = (*a_weights)[j];
            if (val < min)
            {
                index = j;
                min = val;
            }
        }

        const float weight = bone.Weight;
        if (min < weight)
        {   
            (*a_bones)[index] = (float)a_idMap.at(bone.ID) / (float)a_boneCount;
            (*a_weights)[index] = weight;
        }
    }

    delete[] bones;
}

void BlendBoneData(const glm::vec4& a_bonesA, const glm::vec4& a_weightsA, const glm::vec4& a_bonesB, const glm::vec4& a_weightsB, float a_lerp, glm::vec4* a_bones, glm::vec4* a_weights)
{
    glm::vec4 boneData[2] = { glm::vec4(-1) };
    glm::vec4 weightData[2] = { glm::vec4(0) };

    glm::vec4 arrB = a_bonesB; 

    *a_bones = glm::vec4(-1);
    *a_weights = glm::vec4(0);

    const float invLerp = 1 - a_lerp;

    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (a_bonesA[i] == -1)
        {
            continue;
        }

        const int indA = count / 2;
        const int indB = count % 4;

        bool found = false;
        for (int j = 0; j < 4; ++j)
        {
            if (arrB[j] != -1 && a_bonesA[i] == arrB[j])
            {
                found = true;

                arrB[j] = -1;

                boneData[indA][indB] = a_bonesA[i]; 
                weightData[indA][indB] = glm::mix(a_weightsA[i], a_weightsB[i], a_lerp);

                break;
            }
        }

        if (!found)
        {
            boneData[indA][indB] = a_bonesA[i];
            weightData[indA][indB] = glm::mix(a_weightsA[i], 0.0f, a_lerp);
        }

        ++count;
    }

    for (int i = 0; i < 4; ++i)
    {
        if (arrB[i] != -1)
        {
            const int indA = count / 2;
            const int indB = count % 4;

            boneData[indA][indB] = a_bonesB[i];
            weightData[indA][indB] = glm::mix(a_weightsB[i], 0.0f, invLerp);

            ++count;
        }
    }

    for (int i = 0; i < count; ++i)
    {
        float min = std::numeric_limits<float>::infinity();
        int index = 0;

        for (int j = 0; j < 4; ++j)
        {
            const float val = (*a_weights)[j];
            if (val < min)
            {
                index = j;
                min = val;
            }
        }

        const int indA = i / 2;
        const int indB = i % 4;

        const float weight = weightData[indA][indB];
        if (min < weight)
        {
            (*a_bones)[index] = boneData[indA][indB];
            (*a_weights)[index] = weight;
        }
    }
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

        std::unordered_map<long long, unsigned int> idMap;
        const std::list<Object*> armNodes = GetArmatureNodes(); 

        const unsigned int boneCount = armNodes.size();
        unsigned int index = 0;
        for (auto iter = armNodes.begin(); iter != armNodes.end(); ++iter)
        {
            const Object* obj = *iter;
            idMap.emplace(obj->GetID(), index++);
        }

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

                    const glm::vec3 tpL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], 1.0f);
                    const glm::vec3 tpR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], 1.0f);

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

                    for (int i = 0; i < step; ++i)
                    {
                        const float iStep = (float)i / step;
                        const float nIStep = (float)(i + 1) / step;
                        const float bIStep = (float)(i + 2) / step;

                        const glm::vec3 tL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], iStep);
                        const glm::vec3 tR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], iStep);
                        const glm::vec3 mL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], nIStep);
                        const glm::vec3 mR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], nIStep);
                        const glm::vec3 bL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], bIStep);
                        const glm::vec3 bR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], bIStep);

                        const glm::vec2 tLUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], iStep);
                        const glm::vec2 tRUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], iStep);
                        const glm::vec2 mLUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], nIStep);
                        const glm::vec2 mRUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], nIStep);
                        const glm::vec2 bLUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], bIStep);
                        const glm::vec2 bRUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], bIStep);

                        glm::vec4 tLBn = glm::vec4(0);
                        glm::vec4 tRBn = glm::vec4(0);
                        glm::vec4 mLBn = glm::vec4(0);
                        glm::vec4 mRBn = glm::vec4(0);
                        glm::vec4 bLBn = glm::vec4(0);
                        glm::vec4 bRBn = glm::vec4(0);

                        glm::vec4 tLW = glm::vec4(0);
                        glm::vec4 tRW = glm::vec4(0);
                        glm::vec4 mLW = glm::vec4(0);
                        glm::vec4 mRW = glm::vec4(0);
                        glm::vec4 bLW = glm::vec4(0);
                        glm::vec4 bRW = glm::vec4(0);

                        GetBoneData(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], iStep,  &tLW, &tLBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], iStep,  &tRW, &tRBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], nIStep, &mLW, &mLBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], nIStep, &mRW, &mRBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], bIStep, &bLW, &bLBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], bIStep, &bRW, &bRBn, boneCount, idMap);

                        for (int j = 0; j <= i; ++j)
                        {
                            float aS = j / (float)i;
                            if (j <= 0 || i <= 0)
                            {
                                aS = 0;
                            }
                            
                            const float aSMA = j / (float)(i + 1);
                            const float aSMB = (j + 1) / (float)(i + 1);
                            
                            const glm::vec3 tB = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], aS);
                            const glm::vec3 mLB = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], aSMA);
                            const glm::vec3 mRB = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], aSMB);
                            
                            const glm::vec3 t = glm::mix(tL, tR, aS);
                            const glm::vec3 mA = glm::mix(mL, mR, aSMA);
                            const glm::vec3 mB = glm::mix(mL, mR, aSMB); 

                            const glm::vec3 tS = tB - glm::mix(tpL, tpR, aS);
                            const glm::vec3 mSA = mLB - glm::mix(tpL, tpR, aSMA);
                            const glm::vec3 mSB = mRB - glm::mix(tpL, tpR, aSMB);

                            const glm::vec3 tF = t + (tS * iStep);
                            const glm::vec3 mLF = mA + (mSA * nIStep);
                            const glm::vec3 mRF = mB + (mSB * nIStep);

                            const glm::vec2 tFUV = glm::mix(tLUV, tRUV, aS);
                            const glm::vec2 mLFUV = glm::mix(mLUV, mRUV, aSMA);
                            const glm::vec2 mRFUV = glm::mix(mLUV, mRUV, aSMB);

                            glm::vec4 tFB = glm::vec4(0);
                            glm::vec4 mLFB = glm::vec4(0);
                            glm::vec4 mRFB = glm::vec4(0);

                            glm::vec4 tFW = glm::vec4(0);
                            glm::vec4 mLFW = glm::vec4(0);
                            glm::vec4 mRFW = glm::vec4(0);

                            BlendBoneData(tLBn, tLW, tRBn, tRW, aS, &tFB, &tFW);
                            BlendBoneData(mLBn, mLW, mRBn, mRW, aSMA, &mLFB, &mLFW);
                            BlendBoneData(mLBn, mLW, mRBn, mRW, aSMB, &mRFB, &mRFW);

                            for (int i = 0; i < 4; ++i)
                            {
                                if (tFB[i] == -1)
                                {
                                    tFB[i] = 0;
                                    tFW[i] = 0;
                                }

                                if (mLFB[i] == -1)
                                {
                                    mLFB[i] = 0;
                                    mLFW[i] = 0;
                                }

                                if (mRFB[i] == -1)
                                {
                                    mRFB[i] = 0;
                                    mRFW[i] = 0;
                                }
                            }

                            glm::vec3 v1 = mLF - tF;
                            glm::vec3 v2 = mRF - tF;

                            glm::vec3 normal = glm::cross(v2, v1);

                            dirtyVertices.emplace_back(Vertex{ { tF, 1.0f },  normal, tFUV,  tFB,  tFW });
                            dirtyVertices.emplace_back(Vertex{ { mLF, 1.0f }, normal, mLFUV, mLFB, mLFW });
                            dirtyVertices.emplace_back(Vertex{ { mRF, 1.0f }, normal, mRFUV, mRFB, mRFW });

                            if (i < step - 1)
                            {
                                const float aSL = (j + 1) / (float)(i + 2);

                                const glm::vec3 bB = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], aSL);

                                const glm::vec3 b = glm::mix(bL, bR, aSL);

                                const glm::vec3 bS = bB - glm::mix(tpL, tpR, aSL);

                                const glm::vec3 bF = b + (bS * bIStep);

                                const glm::vec2 bFUV = glm::mix(bLUV, bRUV, aSL);

                                glm::vec4 bFB = glm::vec4(0);

                                glm::vec4 bFW = glm::vec4(0);

                                BlendBoneData(bLBn, bLW, bRBn, bRW, aSL, &bFB, &bFW);

                                for (int i = 0; i < 4; ++i)
                                {
                                    if (bFB[i] == -1)
                                    {
                                        bFB[i] = 0;
                                        bFW[i] = 0;
                                    }
                                }

                                v1 = mLF - bF;
                                v2 = mRF - bF;

                                normal = glm::cross(v1, v2);

                                dirtyVertices.emplace_back(Vertex{ { bF, 1.0f },  normal, bFUV,  bFB,  bFW });
                                dirtyVertices.emplace_back(Vertex{ { mRF, 1.0f }, normal, mRFUV, mRFB, mRFW });
                                dirtyVertices.emplace_back(Vertex{ { mLF, 1.0f }, normal, mLFUV, mLFB, mLFW });
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

                        const glm::vec3 pointABLeft =  BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], 2.0f, iStep);
                        const glm::vec3 pointABRight = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], 2.0f, nIStep);
                        const glm::vec3 pointCDLeft =  BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], 2.0f, iStep);
                        const glm::vec3 pointCDRight = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], 2.0f, nIStep);

                        const glm::vec2 pointABLeftUV =  BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], iStep);
                        const glm::vec2 pointABRightUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], nIStep);
                        const glm::vec2 pointCDLeftUV =  BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], iStep);
                        const glm::vec2 pointCDRightUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], nIStep);

                        glm::vec4 pointABLeftBn =  glm::vec4(0);
                        glm::vec4 pointABRightBn = glm::vec4(0);
                        glm::vec4 pointCDLeftBn =  glm::vec4(0);
                        glm::vec4 pointCDRightBn = glm::vec4(0);

                        glm::vec4 pointABLeftW =  glm::vec4(0);
                        glm::vec4 pointABRightW = glm::vec4(0);
                        glm::vec4 pointCDLeftW =  glm::vec4(0);
                        glm::vec4 pointCDRightW = glm::vec4(0);

                        GetBoneData(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], iStep,  &pointABLeftW,  &pointABLeftBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], nIStep, &pointABRightW, &pointABRightBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], iStep,  &pointCDLeftW,  &pointCDLeftBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], nIStep, &pointCDRightW, &pointCDRightBn, boneCount, idMap);

                        for (int j = 0; j < yStep; ++j)
                        {
                            const float jStep = (float)j / yStep;
                            const float nJStep = (float)(j + 1) / yStep;

                            const glm::vec3 pointACLeft =  BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], 2.0f, jStep);
                            const glm::vec3 pointACRight = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], 2.0f, nJStep);
                            const glm::vec3 pointBDLeft =  BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], 2.0f, jStep);
                            const glm::vec3 pointBDRight = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], 2.0f, nJStep);

                            const glm::vec2 pointACLeftUV =  BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], jStep);
                            const glm::vec2 pointACRightUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], nJStep);
                            const glm::vec2 pointBDLeftUV =  BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], jStep);
                            const glm::vec2 pointBDRightUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], nJStep);

                            glm::vec4 pointACLeftBn =  glm::vec4(0);
                            glm::vec4 pointACRightBn = glm::vec4(0);
                            glm::vec4 pointBDLeftBn =  glm::vec4(0);
                            glm::vec4 pointBDRightBn = glm::vec4(0);

                            glm::vec4 pointACLeftW =  glm::vec4(0);
                            glm::vec4 pointACRightW = glm::vec4(0);
                            glm::vec4 pointBDLeftW =  glm::vec4(0);
                            glm::vec4 pointBDRightW = glm::vec4(0);

                            GetBoneData(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], iStep,  &pointACLeftW,  &pointACLeftBn, boneCount, idMap);
                            GetBoneData(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], nIStep, &pointACRightW, &pointACRightBn, boneCount, idMap);
                            GetBoneData(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], iStep,  &pointBDLeftW,  &pointBDLeftBn, boneCount, idMap);
                            GetBoneData(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], nIStep, &pointBDRightW, &pointBDRightBn, boneCount, idMap);

                            const glm::vec3 LLA = glm::mix(pointABLeft,  pointCDLeft,  jStep);
                            const glm::vec3 LHA = glm::mix(pointABLeft,  pointCDLeft,  nJStep);
                            const glm::vec3 HLA = glm::mix(pointABRight, pointCDRight, jStep);
                            const glm::vec3 HHA = glm::mix(pointABRight, pointCDRight, nJStep);

                            const glm::vec3 LLB = glm::mix(pointACLeft,  pointBDLeft,  iStep);
                            const glm::vec3 LHB = glm::mix(pointACLeft,  pointBDLeft,  nIStep);
                            const glm::vec3 HLB = glm::mix(pointACRight, pointBDRight, iStep);
                            const glm::vec3 HHB = glm::mix(pointACRight, pointBDRight, nIStep);

                            const glm::vec2 LLAUV = glm::mix(pointABLeftUV,  pointCDLeftUV,  jStep);
                            const glm::vec2 LHAUV = glm::mix(pointABLeftUV,  pointCDLeftUV,  nJStep);
                            const glm::vec2 HLAUV = glm::mix(pointABRightUV, pointCDRightUV, jStep);
                            const glm::vec2 HHAUV = glm::mix(pointABRightUV, pointCDRightUV, nJStep);

                            const glm::vec2 LLBUV = glm::mix(pointACLeftUV,  pointBDLeftUV,  iStep);
                            const glm::vec2 LHBUV = glm::mix(pointACLeftUV,  pointBDLeftUV,  nIStep);
                            const glm::vec2 HLBUV = glm::mix(pointACRightUV, pointBDRightUV, iStep);
                            const glm::vec2 HHBUV = glm::mix(pointACRightUV, pointBDRightUV, nIStep);

                            glm::vec4 LLABn = glm::vec4(0);
                            glm::vec4 LHABn = glm::vec4(0);
                            glm::vec4 HLABn = glm::vec4(0);
                            glm::vec4 HHABn = glm::vec4(0);

                            glm::vec4 LLBBn = glm::vec4(0);
                            glm::vec4 LHBBn = glm::vec4(0);
                            glm::vec4 HLBBn = glm::vec4(0);
                            glm::vec4 HHBBn = glm::vec4(0);

                            glm::vec4 LLAW = glm::vec4(0);
                            glm::vec4 LHAW = glm::vec4(0);
                            glm::vec4 HLAW = glm::vec4(0);
                            glm::vec4 HHAW = glm::vec4(0);

                            glm::vec4 LLBW = glm::vec4(0);
                            glm::vec4 LHBW = glm::vec4(0);
                            glm::vec4 HLBW = glm::vec4(0);
                            glm::vec4 HHBW = glm::vec4(0);

                            BlendBoneData(pointABLeftBn,  pointABLeftW,  pointCDLeftBn,  pointCDLeftW,  jStep,  &LLABn, &LLAW);
                            BlendBoneData(pointABLeftBn,  pointABLeftW,  pointCDLeftBn,  pointCDLeftW,  nJStep, &LHABn, &LHAW);
                            BlendBoneData(pointABRightBn, pointABRightW, pointCDRightBn, pointCDRightW, jStep,  &HLABn, &HLAW);
                            BlendBoneData(pointABRightBn, pointABRightW, pointCDRightBn, pointCDRightW, nJStep, &HHABn, &HHAW);

                            BlendBoneData(pointACLeftBn,  pointACLeftW,  pointBDLeftBn,  pointBDLeftW,  iStep,  &LLBBn, &LLBW);
                            BlendBoneData(pointACLeftBn,  pointACLeftW,  pointBDLeftBn,  pointBDLeftW,  nIStep, &LHBBn, &LHBW);
                            BlendBoneData(pointABRightBn, pointABRightW, pointCDRightBn, pointCDRightW, iStep,  &HLBBn, &HLBW);
                            BlendBoneData(pointABRightBn, pointABRightW, pointCDRightBn, pointCDRightW, nIStep, &HHBBn, &HHBW);

                            const glm::vec3 posA = (LLA + LLB) * 0.5f;
                            const glm::vec3 posB = (HLA + LHB) * 0.5f;
                            const glm::vec3 posC = (LHA + HLB) * 0.5f;
                            const glm::vec3 posD = (HHA + HHB) * 0.5f;

                            const glm::vec2 uvA = (LLAUV + LLBUV) * 0.5f;
                            const glm::vec2 uvB = (HLAUV + LHBUV) * 0.5f;
                            const glm::vec2 uvC = (LHAUV + HLBUV) * 0.5f;
                            const glm::vec2 uvD = (HHAUV + HHBUV) * 0.5f;

                            glm::vec4 boneA = glm::vec4(0);
                            glm::vec4 boneB = glm::vec4(0);
                            glm::vec4 boneC = glm::vec4(0);
                            glm::vec4 boneD = glm::vec4(0);

                            glm::vec4 weightA = glm::vec4(0);
                            glm::vec4 weightB = glm::vec4(0);
                            glm::vec4 weightC = glm::vec4(0);
                            glm::vec4 weightD = glm::vec4(0);

                            BlendBoneData(LLABn, LLAW, LLBBn, LLBW, 0.5f, &boneA, &weightA);
                            BlendBoneData(HLABn, HLAW, LHBBn, LHBW, 0.5f, &boneB, &weightB);
                            BlendBoneData(LHABn, LHAW, HLBBn, HLBW, 0.5f, &boneC, &weightC);
                            BlendBoneData(HHABn, HHAW, HHBBn, HHBW, 0.5f, &boneD, &weightD);

                            for (int i = 0; i < 4; ++i)
                            {
                                if (boneA[i] == -1)
                                {
                                    boneA[i] = 0;
                                    weightA[i] = 0;
                                }

                                if (boneB[i] == -1)
                                {
                                    boneB[i] = 0;
                                    weightB[i] = 0;
                                }

                                if (boneC[i] == -1)
                                {
                                    boneC[i] = 0;
                                    weightC[i] = 0;
                                }

                                if (boneD[i] == -1)
                                {
                                    boneD[i] = 0;
                                    weightD[i] = 0;
                                }
                            }

                            const glm::vec3 v1 = posB - posA;
                            const glm::vec3 v2 = posC - posA;

                            const glm::vec3 normal = glm::cross(v2, v1);

                            dirtyVertices.emplace_back(Vertex{ { posA, 1.0f }, normal, uvA, boneA, weightA });
                            dirtyVertices.emplace_back(Vertex{ { posB, 1.0f }, normal, uvB, boneB, weightB });
                            dirtyVertices.emplace_back(Vertex{ { posC, 1.0f }, normal, uvC, boneC, weightC });

                            dirtyVertices.emplace_back(Vertex{ { posB, 1.0f }, normal, uvB, boneB, weightB });
                            dirtyVertices.emplace_back(Vertex{ { posD, 1.0f }, normal, uvD, boneD, weightD });
                            dirtyVertices.emplace_back(Vertex{ { posC, 1.0f }, normal, uvC, boneC, weightC });
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
            const double cDist = 1.0f / a_steps * 0.5f;
            const double cDSqr = cDist * cDist;

            for (unsigned int i = 0; i < *a_indexCount; ++i)
            {
                const Vertex vert = dirtyVertices[i];
    
                bool found = false;
    
                for (unsigned int j = 0; j < vertexIndex; ++j)
                {
                    const Vertex cVert = (*a_vertices)[j];

                    const glm::vec3 diff = vert.Position - cVert.Position;

                    if (glm::dot(diff, diff) < cDSqr && vert.UV == cVert.UV)
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
                    const Vertex otherVert = (*a_vertices)[j];
                    if (otherVert.Position == vert.Position && vert.UV == otherVert.UV)
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
    curveModelElement->SetAttribute("Armature", std::to_string(m_armature).c_str());

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
            XMLIO::WriteVec3(a_doc, nElement, "Position", nodes[0].Node.GetPosition());

            for (int j = 0; j < size; ++j)
            {
                const NodeGroup g = nodes[j];

                tinyxml2::XMLElement* cNodeElement = a_doc->NewElement("ClusterNode");
                nElement->InsertEndChild(cNodeElement);

                XMLIO::WriteVec3(a_doc, cNodeElement, "HandlePosition", g.Node.GetHandlePosition());
                XMLIO::WriteVec2(a_doc, cNodeElement, "UV", g.Node.GetUV());

                const std::vector<BoneCluster> bones = g.Node.GetBones();

                if (bones.size() > 0)
                {
                    tinyxml2::XMLElement* bonesElement = a_doc->NewElement("Bones");
                    cNodeElement->InsertEndChild(bonesElement);

                    for (auto iter = bones.begin(); iter != bones.end(); ++iter)
                    {
                        tinyxml2::XMLElement* boneRootElement = a_doc->NewElement("Bone");
                        bonesElement->InsertEndChild(boneRootElement);

                        tinyxml2::XMLElement* boneIndexId = a_doc->NewElement("ID");
                        boneRootElement->InsertEndChild(boneIndexId);

                        boneIndexId->SetText(std::to_string(iter->ID).c_str());

                        if (iter->Weight > 0)
                        {
                            tinyxml2::XMLElement* weight = a_doc->NewElement("Weight");
                            boneRootElement->InsertEndChild(weight);

                            weight->SetText(iter->Weight);
                        }
                    }
                }
            }
        }
    }
}
void CurveModel::ParseData(const tinyxml2::XMLElement* a_element, std::list<BoneGroup>* a_outBones)
{
    m_stepAdjust = a_element->BoolAttribute("StepAdjust");
    m_steps = a_element->IntAttribute("Steps");
    m_armature = std::stoll(a_element->Attribute("Armature"));

    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();

        if (strcmp(str, "Faces") == 0)
        {
            std::vector<CurveFace> faces;

            for (const tinyxml2::XMLElement* fIter = iter->FirstChildElement(); fIter != nullptr; fIter = fIter->NextSiblingElement())
            {
                const char* fStr = fIter->Value();

                if (strcmp(fStr, "Face") == 0)
                {
                    CurveFace face;

                    face.FaceMode = (e_FaceMode)fIter->IntAttribute("FaceMode");

                    int index = 0;
                    int cIndex = 0;
                    for (const tinyxml2::XMLElement* iIter = fIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                    {
                        const char* iStr = iIter->Value();

                        if (strcmp(iStr, "Index") == 0)
                        {
                            face.Index[index++] = iIter->IntText();
                        }
                        else if (strcmp(iStr, "ClusterIndex") == 0)
                        {
                            face.ClusterIndex[cIndex++] = iIter->IntText();
                        }
                        else 
                        {
                            printf("CurveModel::ParseData: InvalidElement: ");
                            printf(iStr);
                            printf("\n");
                        }
                    }

                    faces.emplace_back(face);
                }
                else 
                {
                    printf("CurveModel::ParseData: Invalid Element: ");
                    printf(fStr);
                    printf("\n");
                }
            }

            m_faceCount = faces.size();

            if (m_faces != nullptr)
            {
                delete[] m_faces;
                m_faces = nullptr;
            }

            m_faces = new CurveFace[m_faceCount];

            for (unsigned int i = 0; i < m_faceCount; ++i)
            {
                m_faces[i] = faces[i];
            }
        }
        else if (strcmp(str, "Nodes") == 0)
        {
            std::vector<Node3Cluster> nodes;

            for (const tinyxml2::XMLElement* nIter = iter->FirstChildElement(); nIter != nullptr; nIter = nIter->NextSiblingElement())
            {
                const char* nStr = nIter->Value();

                if (strcmp(nStr, "Node") == 0)
                {
                    Node3Cluster node;

                    glm::vec3 pos = glm::vec3(0);

                    for (const tinyxml2::XMLElement* iIter = nIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                    {
                        const char* iStr = iIter->Value();

                        if (strcmp(iStr, "Position") == 0)
                        {
                            XMLIO::ReadVec3(iIter, &pos);
                        }
                        else if (strcmp(iStr, "ClusterNode") == 0)
                        {
                            NodeGroup n;

                            n.Node.SetPosition(glm::vec3(std::numeric_limits<float>::infinity()));

                            for (const tinyxml2::XMLElement* cIter = iIter->FirstChildElement(); cIter != nullptr; cIter = cIter->NextSiblingElement())
                            {
                                const char* cStr = cIter->Value();

                                if (strcmp(cStr, "HandlePosition") == 0)
                                {
                                    glm::vec3 hPos = glm::vec3(0);

                                    XMLIO::ReadVec3(cIter, &hPos);

                                    n.Node.SetHandlePosition(hPos);
                                }
                                else if (strcmp(cStr, "UV") == 0)
                                {
                                    glm::vec2 uv = glm::vec2(0);

                                    XMLIO::ReadVec2(cIter, &uv);

                                    n.Node.SetUV(uv);
                                }
                                else if (strcmp(cStr, "Bones") == 0)
                                {
                                    for (const tinyxml2::XMLElement* bIter = cIter->FirstChildElement(); bIter != nullptr; bIter = bIter->NextSiblingElement())
                                    {
                                        const char* bStr = bIter->Value();
                                        if (strcmp(bStr, "Bone") == 0)
                                        {
                                            BoneGroup bone;
                                            bone.Index = nodes.size();
                                            bone.ClusterIndex = node.Nodes.size();
                                            bone.ID = 0;
                                            bone.Weight = 0;

                                            for (const tinyxml2::XMLElement* iBIter = bIter->FirstChildElement(); iBIter != nullptr; iBIter = iBIter->NextSiblingElement())
                                            {
                                                const char* iBStr = iBIter->Value();
                                                if (strcmp(iBStr, "ID") == 0)
                                                {
                                                    bone.ID = std::stoll(iBIter->GetText());
                                                }
                                                else if (strcmp(iBStr, "Weight") == 0)
                                                {
                                                    bone.Weight = iBIter->FloatText();
                                                }
                                                else
                                                {
                                                    printf("CurveModel::ParseData: InvalidElement: ");
                                                    printf(iBStr);
                                                    printf("\n");
                                                }
                                            }

                                            a_outBones->emplace_back(bone);
                                        }
                                        else
                                        {
                                            printf("CurveModel::ParseData: InvalidElement: ");
                                            printf(bStr);
                                            printf("\n");
                                        }
                                    }
                                }
                                else
                                {
                                    printf("CurveModel::ParseData: InvalidElement: ");
                                    printf(cStr);
                                    printf("\n");
                                }
                            }

                            node.Nodes.emplace_back(n);
                        }
                        else
                        {
                            printf("CurveModel::ParseData: InvalidElement: ");
                            printf(iStr);
                            printf("\n");
                        }
                    }

                    for (auto iter = node.Nodes.begin(); iter != node.Nodes.end(); ++iter)
                    {
                        if (iter->Node.GetPosition().x == std::numeric_limits<float>::infinity())
                        {
                            iter->Node.SetPosition(pos);
                        }
                    }

                    nodes.emplace_back(node);
                }
                else
                {
                    printf("CurveModel::ParseData: Invalid Element: ");
                    printf(nStr);
                    printf("\n");
                }
            }

            m_nodeCount = nodes.size();

            if (m_nodes != nullptr)
            {
                delete[] m_nodes;
                m_nodes = nullptr;
            }

            m_nodes = new Node3Cluster[m_nodeCount];

            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                m_nodes[i] = nodes[i];
            }
        }
        else
        {
            printf("CurveModel::ParseData: Invalid Element: ");
            printf(str);
            printf("\n");
        }
    }

    for (unsigned int i = 0; i < m_faceCount; ++i)
    {
        const CurveFace face = m_faces[i];

        switch (face.FaceMode)
        {
        case FaceMode_3Point:
        {
            for (int i = 0; i < 6; ++i)
            {
                ++m_nodes[face.Index[i]].Nodes[face.ClusterIndex[i]].FaceCount;
            }

            break;
        }
        case FaceMode_4Point:
        {
            for (int i = 0; i < 8; ++i)
            {
                ++m_nodes[face.Index[i]].Nodes[face.ClusterIndex[i]].FaceCount;
            }

            break;
        }
        }
    }
}
void CurveModel::PostParseData(const std::list<BoneGroup>& a_bones, const std::unordered_map<long long, long long>& a_idMap)
{
    auto armID = a_idMap.find(m_armature);

    if (armID != a_idMap.end())
    {
        m_armature = armID->second;
    }
    else
    {
        m_armature = -1;
    }

    for (auto iter = a_bones.begin(); iter != a_bones.end(); ++iter)
    {
        const unsigned int size = m_nodes[iter->Index].Nodes.size();

        auto idIndex = a_idMap.find(iter->ID);
        if (idIndex != a_idMap.end())
        {
            m_nodes[iter->Index].Nodes[iter->ClusterIndex].Node.SetBoneWeight(idIndex->second, iter->Weight);
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

        a_file->write("vt", 2);
        for (int j = 0; j < 2; ++j)
        {
            const std::string str = " " + std::to_string(vert.UV[j]);
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
            const std::string str = " " + strVal + "/" + strVal + '/' + strVal;
            a_file->write(str.c_str(), str.length());
        }

        a_file->write(" \n", 2);    
    }
}