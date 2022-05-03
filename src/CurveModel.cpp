#include "CurveModel.h"

#include <unordered_map>

#include "Model.h"
#include "Object.h"
#include "Transform.h"
#include "Workspace.h"

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
unsigned int CurveModel::GetArmatureNodeCount() const
{
    return GetArmatureNodes().size();
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

void CurveModel::EmplaceNode(unsigned int a_index, const CurveNodeCluster& a_node)
{
    const unsigned int size = m_nodeCount + 1;

    CurveNodeCluster* newNodes = new CurveNodeCluster[size];

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
void CurveModel::EmplaceNode(const CurveNodeCluster& a_node)
{
    EmplaceNodes(&a_node, 1);
}
void CurveModel::EmplaceNodes(const CurveNodeCluster* a_nodes, unsigned int a_count)
{
    const unsigned int size = m_nodeCount + a_count;

    CurveNodeCluster* newNodes = new CurveNodeCluster[size];

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

    CurveNodeCluster* newNodes = new CurveNodeCluster[size];

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

void CurveModel::SetModelData(const CurveNodeCluster* a_nodes, unsigned int a_nodeCount, const CurveFace* a_faces, unsigned int a_faceCount)
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

    m_nodeCount = a_nodeCount;
    m_faceCount = a_faceCount;

    m_nodes = new CurveNodeCluster[m_nodeCount];
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodes[i] = a_nodes[i];
    }

    m_faces = new CurveFace[m_faceCount];
    for (unsigned int i = 0; i < m_faceCount; ++i)
    {
        m_faces[i] = a_faces[i];
    }
}
void CurveModel::PassModelData(CurveNodeCluster* a_nodes, unsigned int a_nodeCount, CurveFace* a_faces, unsigned int a_faceCount)
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

    delete[] vertices;
    delete[] indices;
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

float BlendWeight(float a_weightA, float a_weightB, float a_lerp)
{
    const float step = glm::mix(a_weightA, a_weightB, a_lerp);

    const float innerStepA = glm::mix(a_weightA, step, a_lerp);
    const float innerStepB = glm::mix(step, a_weightB, a_lerp);

    return glm::mix(innerStepA, innerStepB, a_lerp);
}
void BlendBoneDataNL(const glm::vec4& a_bonesA, const glm::vec4& a_weightsA, const glm::vec4& a_bonesB, const glm::vec4& a_weightsB, float a_lerp, glm::vec4* a_bones, glm::vec4* a_weights)
{
    glm::vec4 boneData[2] = { glm::vec4(-1) };
    glm::vec4 weightData[2] = { glm::vec4(0) };

    glm::vec4 arrB = a_bonesB; 

    *a_bones = glm::vec4(-1);
    *a_weights = glm::vec4(0);

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
                weightData[indA][indB] = BlendWeight(a_weightsA[i], a_weightsB[i], a_lerp);

                break;
            }
        }

        if (!found)
        {
            boneData[indA][indB] = a_bonesA[i];
            weightData[indA][indB] = BlendWeight(a_weightsA[i], 0.0f, a_lerp);
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
            weightData[indA][indB] = BlendWeight(0.0f, a_weightsB[i], a_lerp);

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
void BlendBoneData(const glm::vec4& a_bonesA, const glm::vec4& a_weightsA, const glm::vec4& a_bonesB, const glm::vec4& a_weightsB, float a_lerp, glm::vec4* a_bones, glm::vec4* a_weights)
{
    glm::vec4 boneData[2] = { glm::vec4(-1) };
    glm::vec4 weightData[2] = { glm::vec4(0) };

    glm::vec4 arrB = a_bonesB; 

    *a_bones = glm::vec4(-1);
    *a_weights = glm::vec4(0);

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
            weightData[indA][indB] = glm::mix(0.0f, a_weightsB[i], a_lerp);

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

glm::vec2 BlendUV(const glm::vec2& a_start, const glm::vec2& a_end, float a_lerp)
{
    const glm::vec2 step = glm::mix(a_start, a_end, a_lerp);

    const glm::vec2 innerStepA = glm::mix(a_start, step, a_lerp);
    const glm::vec2 innerStepB = glm::mix(step, a_end, a_lerp);

    return glm::mix(innerStepA, innerStepB, a_lerp);
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

                    const unsigned int indexA = face.Index[FaceIndex_3Point_AB];
                    const unsigned int indexB = face.Index[FaceIndex_3Point_BC];
                    const unsigned int indexC = face.Index[FaceIndex_3Point_CA];

                    const glm::vec4 bodyI = glm::vec4((float)indexA / m_nodeCount, (float)indexB / m_nodeCount, (float)indexC / m_nodeCount, 0.0f);

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

                        glm::vec4 tLBn = glm::vec4(0.0f);
                        glm::vec4 tRBn = glm::vec4(0.0f);
                        glm::vec4 mLBn = glm::vec4(0.0f);
                        glm::vec4 mRBn = glm::vec4(0.0f);
                        glm::vec4 bLBn = glm::vec4(0.0f);
                        glm::vec4 bRBn = glm::vec4(0.0f);

                        glm::vec4 tLW = glm::vec4(0.0f);
                        glm::vec4 tRW = glm::vec4(0.0f);
                        glm::vec4 mLW = glm::vec4(0.0f);
                        glm::vec4 mRW = glm::vec4(0.0f);
                        glm::vec4 bLW = glm::vec4(0.0f);
                        glm::vec4 bRW = glm::vec4(0.0f);

                        GetBoneData(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], iStep,  &tLW, &tLBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], iStep,  &tRW, &tRBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], nIStep, &mLW, &mLBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], nIStep, &mRW, &mRBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], bIStep, &bLW, &bLBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], bIStep, &bRW, &bRBn, boneCount, idMap);

                        const float bdTL = glm::mix(0.0f, iStep, iStep);
                        const float bdTR = glm::mix(iStep, 1.0f, iStep);
                        const float bdTM = glm::mix(bdTL, bdTR, iStep);
                        const float bdTMI = 1.0f - bdTM;

                        const float bdML = glm::mix(0.0f, nIStep, nIStep);
                        const float bdMR = glm::mix(nIStep, 1.0f, nIStep);
                        const float bdMM = glm::mix(bdML, bdMR, nIStep);
                        const float bdMMI = 1.0f - bdMM;

                        const float bdBL = glm::mix(0.0f, bIStep, bIStep);
                        const float bdBR = glm::mix(bIStep, 1.0f, bIStep);
                        const float bdBM = glm::mix(bdBL, bdBR, bIStep);
                        const float bdBMI = 1.0f - bdBM;

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

                            const glm::vec2 tFUV = BlendUV(tLUV, tRUV, aS);
                            const glm::vec2 mLFUV = BlendUV(mLUV, mRUV, aSMA);
                            const glm::vec2 mRFUV = BlendUV(mLUV, mRUV, aSMB);

                            glm::vec4 tFB = glm::vec4(0.0f);
                            glm::vec4 mLFB = glm::vec4(0.0f);
                            glm::vec4 mRFB = glm::vec4(0.0f);

                            glm::vec4 tFW = glm::vec4(0.0f);
                            glm::vec4 mLFW = glm::vec4(0.0f);
                            glm::vec4 mRFW = glm::vec4(0.0f);

                            BlendBoneDataNL(tLBn, tLW, tRBn, tRW, aS, &tFB, &tFW);
                            BlendBoneDataNL(mLBn, mLW, mRBn, mRW, aSMA, &mLFB, &mLFW);
                            BlendBoneDataNL(mLBn, mLW, mRBn, mRW, aSMB, &mRFB, &mRFW);

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

                            const float bdASL = glm::mix(0.0f, aS, aS);
                            const float bdASR = glm::mix(aS, 1.0f, aS);
                            const float bdASM = glm::mix(bdASL, bdASR, aS);
                            const float bdASMI = 1.0f - bdASM;

                            const float bdASMAL = glm::mix(0.0f, aSMA, aSMA);
                            const float bdASMAR = glm::mix(aSMA, 1.0f, aSMA);
                            const float bdASMAM = glm::mix(bdASMAL, bdASMAR, aSMA);
                            const float bdASMAMI = 1.0f - bdASMAM;

                            const float bdASMBL = glm::mix(0.0f, aSMB, aSMB);
                            const float bdASMBR = glm::mix(aSMB, 1.0f, aSMB);
                            const float bdASMBM = glm::mix(bdASMBL, bdASMBR, aSMB);
                            const float bdASMBMI = 1.0f - bdASMBM;

                            const glm::vec4 tFSW = glm::vec4(bdTM * 0.66f, (bdTMI + bdASM) * 0.33f, (bdTMI + bdASMI) * 0.33f, 0.0f);
                            const glm::vec4 mLFSW = glm::vec4(bdMM * 0.66f, (bdMMI + bdASMAM) * 0.33f, (bdMMI + bdASMAMI) * 0.33f, 0.0f);
                            const glm::vec4 mRFSW = glm::vec4(bdMM * 0.66f, (bdMMI + bdASMBM) * 0.33f, (bdMMI + bdASMBMI) * 0.33f, 0.0f);

                            dirtyVertices.emplace_back(Vertex{ { tF, 1.0f },  normal, tFUV,  tFB,  tFW,  bodyI, tFSW });
                            dirtyVertices.emplace_back(Vertex{ { mLF, 1.0f }, normal, mLFUV, mLFB, mLFW, bodyI, mLFSW });
                            dirtyVertices.emplace_back(Vertex{ { mRF, 1.0f }, normal, mRFUV, mRFB, mRFW, bodyI, mRFSW });

                            if (i < step - 1)
                            {
                                const float aSL = (j + 1) / (float)(i + 2);

                                const float bdASLL = glm::mix(0.0f, aSL, aSL);
                                const float bdASLR = glm::mix(aSL, 1.0f, aSL);
                                const float bdASLM = glm::mix(bdASLL, bdASLR, aSL);
                                const float bdASLMI = 1.0f - bdASLM;

                                const glm::vec3 bB = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], aSL);

                                const glm::vec3 b = glm::mix(bL, bR, aSL);

                                const glm::vec3 bS = bB - glm::mix(tpL, tpR, aSL);

                                const glm::vec3 bF = b + (bS * bIStep);

                                const glm::vec2 bFUV = BlendUV(bLUV, bRUV, aSL);

                                glm::vec4 bFB = glm::vec4(0);

                                glm::vec4 bFW = glm::vec4(0);

                                BlendBoneDataNL(bLBn, bLW, bRBn, bRW, aSL, &bFB, &bFW);

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

                                const glm::vec4 bFSW = glm::vec4(bdBM * 0.66f, (bdBMI + bdASLM) * 0.33f, (bdBMI + bdASLMI) * 0.33f, 0.0f);

                                dirtyVertices.emplace_back(Vertex{ { bF, 1.0f },  normal, bFUV,  bFB,  bFW,  bodyI, bFSW });
                                dirtyVertices.emplace_back(Vertex{ { mRF, 1.0f }, normal, mRFUV, mRFB, mRFW, bodyI, mRFSW });
                                dirtyVertices.emplace_back(Vertex{ { mLF, 1.0f }, normal, mLFUV, mLFB, mLFW, bodyI, mLFSW });
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

                    const unsigned int indexA = face.Index[FaceIndex_4Point_AB];
                    const unsigned int indexB = face.Index[FaceIndex_4Point_BD];
                    const unsigned int indexC = face.Index[FaceIndex_4Point_DC];
                    const unsigned int indexD = face.Index[FaceIndex_4Point_CA];

                    const glm::vec4 bodyI = glm::vec4((float)indexA / m_nodeCount, (float)indexB / m_nodeCount, (float)indexC / m_nodeCount, (float)indexD / m_nodeCount);

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

                        glm::vec4 pointABLeftBn =  glm::vec4(0.0f);
                        glm::vec4 pointABRightBn = glm::vec4(0.0f);
                        glm::vec4 pointCDLeftBn =  glm::vec4(0.0f);
                        glm::vec4 pointCDRightBn = glm::vec4(0.0f);

                        glm::vec4 pointABLeftW =  glm::vec4(0.0f);
                        glm::vec4 pointABRightW = glm::vec4(0.0f);
                        glm::vec4 pointCDLeftW =  glm::vec4(0.0f);
                        glm::vec4 pointCDRightW = glm::vec4(0.0f);

                        GetBoneData(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], iStep,  &pointABLeftW,  &pointABLeftBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], nIStep, &pointABRightW, &pointABRightBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], iStep,  &pointCDLeftW,  &pointCDLeftBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], nIStep, &pointCDRightW, &pointCDRightBn, boneCount, idMap);

                        const float bdLL = glm::mix(0.0f, iStep, iStep);
                        const float bdLR = glm::mix(iStep, 1.0f, iStep);
                        const float bdLM = glm::mix(bdLL, bdLR, iStep);
                        const float bdLMI = 1.0f - bdLM;

                        const float bdRL = glm::mix(0.0f, nIStep, nIStep);
                        const float bdRR = glm::mix(nIStep, 1.0f, nIStep);
                        const float bdRM = glm::mix(bdRL, bdRR, nIStep);
                        const float bdRMI = 1.0f - bdRM;

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

                            glm::vec4 pointACLeftBn =  glm::vec4(0.0f);
                            glm::vec4 pointACRightBn = glm::vec4(0.0f);
                            glm::vec4 pointBDLeftBn =  glm::vec4(0.0f);
                            glm::vec4 pointBDRightBn = glm::vec4(0.0f);

                            glm::vec4 pointACLeftW =  glm::vec4(0.0f);
                            glm::vec4 pointACRightW = glm::vec4(0.0f);
                            glm::vec4 pointBDLeftW =  glm::vec4(0.0f);
                            glm::vec4 pointBDRightW = glm::vec4(0.0f);

                            GetBoneData(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], iStep,  &pointACLeftW,  &pointACLeftBn, boneCount, idMap);
                            GetBoneData(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], nIStep, &pointACRightW, &pointACRightBn, boneCount, idMap);
                            GetBoneData(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], iStep,  &pointBDLeftW,  &pointBDLeftBn, boneCount, idMap);
                            GetBoneData(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], nIStep, &pointBDRightW, &pointBDRightBn, boneCount, idMap);

                            const float bdTL = glm::mix(0.0f, jStep, jStep);
                            const float bdTR = glm::mix(jStep, 1.0f, jStep);
                            const float bdTM = glm::mix(bdTL, bdTR, jStep);
                            const float bdTMI = 1.0f - bdTM;

                            const float bdBL = glm::mix(0.0f, nJStep, nJStep);
                            const float bdBR = glm::mix(nJStep, 1.0f, nJStep);
                            const float bdBM = glm::mix(bdBL, bdBR, nJStep);
                            const float bdBMI = 1.0f - bdBM;

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

                            glm::vec4 LLABn = glm::vec4(0.0f);
                            glm::vec4 LHABn = glm::vec4(0.0f);
                            glm::vec4 HLABn = glm::vec4(0.0f);
                            glm::vec4 HHABn = glm::vec4(0.0f);

                            glm::vec4 LLBBn = glm::vec4(0.0f);
                            glm::vec4 LHBBn = glm::vec4(0.0f);
                            glm::vec4 HLBBn = glm::vec4(0.0f);
                            glm::vec4 HHBBn = glm::vec4(0.0f);

                            glm::vec4 LLAW = glm::vec4(0.0f);
                            glm::vec4 LHAW = glm::vec4(0.0f);
                            glm::vec4 HLAW = glm::vec4(0.0f);
                            glm::vec4 HHAW = glm::vec4(0.0f);

                            glm::vec4 LLBW = glm::vec4(0.0f);
                            glm::vec4 LHBW = glm::vec4(0.0f);
                            glm::vec4 HLBW = glm::vec4(0.0f);
                            glm::vec4 HHBW = glm::vec4(0.0f);

                            BlendBoneDataNL(pointABLeftBn,  pointABLeftW,  pointCDLeftBn,  pointCDLeftW,  jStep,  &LLABn, &LLAW);
                            BlendBoneDataNL(pointABLeftBn,  pointABLeftW,  pointCDLeftBn,  pointCDLeftW,  nJStep, &LHABn, &LHAW);
                            BlendBoneDataNL(pointABRightBn, pointABRightW, pointCDRightBn, pointCDRightW, jStep,  &HLABn, &HLAW);
                            BlendBoneDataNL(pointABRightBn, pointABRightW, pointCDRightBn, pointCDRightW, nJStep, &HHABn, &HHAW);

                            BlendBoneDataNL(pointACLeftBn,  pointACLeftW,  pointBDLeftBn,  pointBDLeftW,  iStep,  &LLBBn, &LLBW);
                            BlendBoneDataNL(pointACLeftBn,  pointACLeftW,  pointBDLeftBn,  pointBDLeftW,  nIStep, &LHBBn, &LHBW);
                            BlendBoneDataNL(pointABRightBn, pointABRightW, pointCDRightBn, pointCDRightW, iStep,  &HLBBn, &HLBW);
                            BlendBoneDataNL(pointABRightBn, pointABRightW, pointCDRightBn, pointCDRightW, nIStep, &HHBBn, &HHBW);

                            const glm::vec3 posA = (LLA + LLB) * 0.5f;
                            const glm::vec3 posB = (HLA + LHB) * 0.5f;
                            const glm::vec3 posC = (LHA + HLB) * 0.5f;
                            const glm::vec3 posD = (HHA + HHB) * 0.5f;

                            const glm::vec2 uvA = (LLAUV + LLBUV) * 0.5f;
                            const glm::vec2 uvB = (HLAUV + LHBUV) * 0.5f;
                            const glm::vec2 uvC = (LHAUV + HLBUV) * 0.5f;
                            const glm::vec2 uvD = (HHAUV + HHBUV) * 0.5f;

                            glm::vec4 boneA = glm::vec4(0.0f);
                            glm::vec4 boneB = glm::vec4(0.0f);
                            glm::vec4 boneC = glm::vec4(0.0f);
                            glm::vec4 boneD = glm::vec4(0.0f);

                            glm::vec4 weightA = glm::vec4(0.0f);
                            glm::vec4 weightB = glm::vec4(0.0f);
                            glm::vec4 weightC = glm::vec4(0.0f);
                            glm::vec4 weightD = glm::vec4(0.0f);

                            const glm::vec4 bodyWA = glm::vec4((bdLM + bdTM) * 0.25f, (bdLMI + bdTM) * 0.25f, (bdLMI + bdTMI) * 0.25f, (bdLM + bdTMI) * 0.25f);
                            const glm::vec4 bodyWB = glm::vec4((bdRM + bdTM) * 0.25f, (bdRMI + bdTM) * 0.25f, (bdRMI + bdTMI) * 0.25f, (bdRM + bdTMI) * 0.25f);
                            const glm::vec4 bodyWC = glm::vec4((bdRM + bdBM) * 0.25f, (bdRMI + bdBM) * 0.25f, (bdRMI + bdBMI) * 0.25f, (bdRM + bdBMI) * 0.25f);
                            const glm::vec4 bodyWD = glm::vec4((bdLM + bdBM) * 0.25f, (bdLMI + bdBM) * 0.25f, (bdLMI + bdBMI) * 0.25f, (bdLM + bdBMI) * 0.25f);;

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

                            dirtyVertices.emplace_back(Vertex{ { posA, 1.0f }, normal, uvA, boneA, weightA, bodyI, bodyWA });
                            dirtyVertices.emplace_back(Vertex{ { posB, 1.0f }, normal, uvB, boneB, weightB, bodyI, bodyWB });
                            dirtyVertices.emplace_back(Vertex{ { posC, 1.0f }, normal, uvC, boneC, weightC, bodyI, bodyWC });

                            dirtyVertices.emplace_back(Vertex{ { posB, 1.0f }, normal, uvB, boneB, weightB, bodyI, bodyWB });
                            dirtyVertices.emplace_back(Vertex{ { posD, 1.0f }, normal, uvD, boneD, weightD, bodyI, bodyWD });
                            dirtyVertices.emplace_back(Vertex{ { posC, 1.0f }, normal, uvC, boneC, weightC, bodyI, bodyWC });
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
    
                for (unsigned int j = 0; j < vertexIndex; ++j)
                {
                    const Vertex cVert = (*a_vertices)[j];

                    const glm::vec3 diff = vert.Position - cVert.Position;
                    const glm::vec2 uvDiff = vert.UV - cVert.UV;

                    // Using an epsilon because floating point precision was causing issues
                    if (glm::dot(diff, diff) < cDSqr && glm::dot(uvDiff, uvDiff) <= 0.0001f)
                    {
                        (*a_vertices)[j].Normal += vert.Normal;
                        (*a_indices)[i] = j;
    
                        goto SmartNext;
                    }
                }
    
                (*a_vertices)[vertexIndex] = vert;
                (*a_indices)[i] = vertexIndex++;
SmartNext:; 
            }
        }
        else
        {
            for (unsigned int i = 0; i < *a_indexCount; ++i)
            {
                const Vertex vert = dirtyVertices[i];

                for (unsigned int j = 0; j < vertexIndex; ++j)
                {
                    const Vertex otherVert = (*a_vertices)[j];

                    const glm::vec3 diff = vert.Position - otherVert.Position;
                    const glm::vec2 uvDiff = vert.UV - otherVert.UV;

                    // Using an epsilon because floating point precision was causing issues
                    if (glm::dot(diff, diff) <= 0.0001f && glm::dot(uvDiff, uvDiff) <= 0.0001f)
                    {
                        (*a_vertices)[j].Normal += vert.Normal;
                        (*a_indices)[i] = j;

                        goto Next;
                    }
                }

                (*a_vertices)[vertexIndex] = vert;
                (*a_indices)[i] = vertexIndex++;
Next:;
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
void CurveModel::PostTriangulate(const unsigned int* a_indices, unsigned int a_indexCount, const Vertex* a_vertices, unsigned int a_vertexCount)
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
}
