#include "CurveModel.h"

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

glm::vec3 CurveModel::GetMirrorMultiplier(e_MirrorMode a_mode) const
{
    glm::vec3 mul = glm::vec3(1.0f);

    if (a_mode & MirrorMode_X)
    {
        mul.x = -1.0f;
    }
    if (a_mode & MirrorMode_Y)
    {
        mul.y = -1.0f;
    }
    if (a_mode & MirrorMode_Z)
    {
        mul.z = -1.0f;
    }

    return mul;
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

float CurveModel::GetNodeDist(const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB) const
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

unsigned int* CurveModel::GetMirroredIndices(unsigned int a_index, e_MirrorMode a_mirrorMode) const
{
    unsigned int* indices = new unsigned int[7];

    const bool xMode = (a_mirrorMode & MirrorMode_X) != 0;
    const bool yMode = (a_mirrorMode & MirrorMode_Y) != 0;
    const bool zMode = (a_mirrorMode & MirrorMode_Z) != 0;

    const glm::vec3 pos = m_nodes[a_index].Nodes[0].Node.GetPosition();

    for (int i = 0; i < 7; ++i)
    {
        indices[i] = -1;

        const e_MirrorMode mode = (e_MirrorMode)(i + 1);
        
        if ((mode & MirrorMode_X && !xMode) ||
            (mode & MirrorMode_Y && !yMode) ||
            (mode & MirrorMode_Z && !zMode))
        {
            continue;
        }

        const glm::vec3 mul = GetMirrorMultiplier(mode);

        const glm::vec3 invPos = pos * mul;

        for (unsigned int j = 0; j < m_nodeCount; ++j)
        {
            if (j == a_index)
            {
                continue;
            }

            const glm::vec3 pos = m_nodes[j].Nodes[0].Node.GetPosition();

            if (glm::length(pos - invPos) <= 0.0001f)
            {
                indices[i] = j;

                break;
            }
        }
    }

    return indices;
}
void CurveModel::GetMirroredHandles(unsigned int a_index, unsigned int a_nodeIndex, e_MirrorMode a_mode, unsigned int** a_outIndices, unsigned int** a_outNodeIndices) const
{
    *a_outIndices = new unsigned int[7];
    *a_outNodeIndices = new unsigned int[7];

    const bool xMode = (a_mode & MirrorMode_X) != 0;
    const bool yMode = (a_mode & MirrorMode_Y) != 0;
    const bool zMode = (a_mode & MirrorMode_Z) != 0;

    const BezierCurveNode3& curve = m_nodes[a_index].Nodes[a_nodeIndex].Node;
    const glm::vec3 pos = curve.GetPosition();
    const glm::vec3 hPos = curve.GetHandlePosition();

    for (int i = 0; i < 7; ++i)
    {
        (*a_outIndices)[i] = -1;
        (*a_outNodeIndices)[i] = -1;

        const e_MirrorMode mode = (e_MirrorMode)(i + 1);

        if ((mode & MirrorMode_X && !xMode) ||
            (mode & MirrorMode_Y && !yMode) ||
            (mode & MirrorMode_Z && !zMode))
        {
            continue;
        }

        const glm::vec3 mul = GetMirrorMultiplier(mode);

        const glm::vec3 invPos = pos * mul;
        const glm::vec3 invHPos = hPos * mul;

        for (unsigned int j = 0; j < m_nodeCount; ++j)
        {
            const CurveNodeCluster& c = m_nodes[j];
            const glm::vec3 pos = c.Nodes[0].Node.GetPosition();

            if (glm::length(pos - invPos) <= 0.0001f)
            {
                const unsigned int size = (unsigned int)c.Nodes.size();

                for (unsigned int k = 0; k < size; ++k)
                {
                    const glm::vec3 hPos = c.Nodes[k].Node.GetHandlePosition();
                    if (glm::length(hPos - invHPos) <= 0.0001f)
                    {
                        (*a_outIndices)[i] = j;
                        (*a_outNodeIndices)[i] = k;

                        goto NextIter;
                    }
                }
            }
        }

NextIter:;
    }
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

void CurveModel::GetBoneData(const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB, float a_lerp, glm::vec4* a_weights, glm::vec4* a_bones, unsigned int a_boneCount, const std::unordered_map<long long, unsigned int>& a_idMap) const
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

float CurveModel::BlendWeight(float a_weightA, float a_weightB, float a_lerp) const
{
    const float step = glm::mix(a_weightA, a_weightB, a_lerp);

    const float innerStepA = glm::mix(a_weightA, step, a_lerp);
    const float innerStepB = glm::mix(step, a_weightB, a_lerp);

    return glm::mix(innerStepA, innerStepB, a_lerp);
}
void CurveModel::BlendBoneDataNL(const glm::vec4& a_bonesA, const glm::vec4& a_weightsA, const glm::vec4& a_bonesB, const glm::vec4& a_weightsB, float a_lerp, glm::vec4* a_bones, glm::vec4* a_weights) const
{
    glm::vec4 boneData[2] = { glm::vec4(-1.0f) };
    glm::vec4 weightData[2] = { glm::vec4(0.0f) };

    glm::vec4 arrB = a_bonesB; 

    *a_bones = glm::vec4(-1.0f);
    *a_weights = glm::vec4(0.0f);

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
void CurveModel::BlendBoneData(const glm::vec4& a_bonesA, const glm::vec4& a_weightsA, const glm::vec4& a_bonesB, const glm::vec4& a_weightsB, float a_lerp, glm::vec4* a_bones, glm::vec4* a_weights) const
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

glm::vec2 CurveModel::BlendUV(const glm::vec2& a_start, const glm::vec2& a_end, float a_lerp) const
{
    const glm::vec2 step = glm::mix(a_start, a_end, a_lerp);

    const glm::vec2 innerStepA = glm::mix(a_start, step, a_lerp);
    const glm::vec2 innerStepB = glm::mix(step, a_end, a_lerp);

    return glm::mix(innerStepA, innerStepB, a_lerp);
}

unsigned int CurveModel::GetEdge(std::vector<CurveEdge>& a_edge, unsigned int a_indexA, unsigned int a_indexB, char* a_index) const
{
    *a_index = 0;

    const unsigned int size = (unsigned int)a_edge.size();
    for (unsigned int i = 0; i < size; ++i)
    {
        CurveEdge& edge = a_edge[i];

        if ((edge.IndexL == a_indexA && edge.IndexH == a_indexB) || (edge.IndexL == a_indexB && edge.IndexH == a_indexA))
        {
            *a_index = 1;

            return i;
        } 
    }

    CurveEdge edge;
    
    edge.SetCount[0] = 0;
    edge.SetCount[1] = 0;
    edge.Set[0] = nullptr;
    edge.Set[1] = nullptr;
    edge.IndexL = a_indexA;
    edge.IndexH = a_indexB;

    a_edge.emplace_back(edge);

    return size;
}

void CurveModel::GetModelData(bool a_smartStep, int a_steps, unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const
{
    constexpr float infinity = std::numeric_limits<float>::infinity();

    *a_vertices = nullptr;
    *a_indices = nullptr;
    *a_indexCount = 0;
    *a_vertexCount = 0;

    if (m_faceCount > 0)
    {
        std::vector<Vertex> dirtyVertices;
        std::vector<unsigned int> dirtyIndices;

        std::vector<CurveEdge> dirtyEdges;

        std::unordered_map<long long, unsigned int> idMap;
        const std::list<Object*> armNodes = GetArmatureNodes(); 

        const unsigned int boneCount = armNodes.size();
        unsigned int index = 0;
        for (auto iter = armNodes.begin(); iter != armNodes.end(); ++iter)
        {
            const Object* obj = *iter;
            idMap.emplace(obj->GetID(), index++);
        }

        for (int i = 0; i < m_faceCount; ++i)
        {
            const CurveFace& face = m_faces[i];

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

                int step = a_steps;
                if (a_smartStep)
                {
                    const float aDist = GetNodeDist(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA]);
                    const float bDist = GetNodeDist(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB]);
                    const float cDist = GetNodeDist(nodes[FaceIndex_3Point_CA], nodes[FaceIndex_3Point_AC]);

                    const float m = glm::max(aDist, glm::max(bDist, cDist));
                    step = (int)glm::ceil(m * a_steps * 0.5f);
                }

                const int size = step + 1;
                const unsigned int start = (unsigned int)dirtyVertices.size();

                for (int i = 0; i <= step; ++i)
                {
                    const int nI = i + 1;
                    const int hI = nI + 1;

                    const float iStep = (float)i / step;
                    const float nIStep = (float)nI / step;

                    const glm::vec3 vL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], iStep);
                    const glm::vec3 vR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], iStep);

                    const glm::vec3 vLL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], nIStep);
                    const glm::vec3 vLR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_3Point_CA], nIStep);

                    const glm::vec2 vUVL = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], iStep);
                    const glm::vec2 vUVR = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_4Point_CA], iStep);

                    glm::vec4 lBn;
                    glm::vec4 rBn;
                    glm::vec4 lW;
                    glm::vec4 rW;

                    GetBoneData(nodes[FaceIndex_3Point_AB], nodes[FaceIndex_3Point_BA], iStep, &lW, &lBn, boneCount, idMap);
                    GetBoneData(nodes[FaceIndex_3Point_AC], nodes[FaceIndex_4Point_CA], iStep, &rW, &rBn, boneCount, idMap);

                    for (int j = 0; j <= i; ++j)
                    {
                        const int nJ = j + 1;

                        float aS = j / (float)i;
                        if (j <= 0 || i <= 0)
                        {
                            aS = 0.0f;
                        }

                        const float aSMA = j / (float)nI;
                        const float aSMB = nJ / (float)nI;

                        const glm::vec3 h = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], aS);
                        const glm::vec3 hL = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], aSMA);
                        const glm::vec3 hR = BezierCurveNode3::GetPoint(nodes[FaceIndex_3Point_BC], nodes[FaceIndex_3Point_CB], aSMB);
                        
                        const glm::vec3 v = glm::mix(vL, vR, aS);
                        const glm::vec3 vL = glm::mix(vLL, vLR, aSMA);
                        const glm::vec3 vR = glm::mix(vLL, vLR, aSMB);
                        
                        const glm::vec3 vS = h - glm::mix(tpL, tpR, aS);
                        const glm::vec3 vSL = hL - glm::mix(tpL, tpR, aSMA);
                        const glm::vec3 vSR = hR - glm::mix(tpL, tpR, aSMB);

                        const glm::vec3 point = v + (vS * iStep);
                        const glm::vec3 pA = vL + (vSL * nIStep);
                        const glm::vec3 pB = vR + (vSR * nIStep);

                        const glm::vec3 v1 = pA - point;
                        const glm::vec3 v2 = pB - point;

                        const glm::vec3 normal = glm::normalize(glm::cross(v2, v1));

                        const glm::vec2 uv = BlendUV(vUVL, vUVR, aS);

                        glm::vec4 bone;
                        glm::vec4 weight;

                        BlendBoneDataNL(lBn, lW, rBn, rW, aS, &bone, &weight);

                        for (int i = 0; i < 4; ++i)
                        {
                            if (bone[i] < 0)
                            {
                                bone[i] = 0;
                                weight[i] = 0;
                            }
                        }

                        dirtyVertices.emplace_back(Vertex{ glm::vec4(point, 1.0f), normal, uv, bone, weight, bodyI, glm::vec4(0.0f) });

                        if (i < step)
                        {
                            const unsigned int lOff = (unsigned int)((i * 0.5f) * (i + 1));
                            const unsigned int mOff = (unsigned int)((nI * 0.5f) * (nI + 1));

                            const unsigned int indexA = start + lOff + j;
                            const unsigned int indexB = start + mOff + j;
                            const unsigned int indexC = start + mOff + nJ;

                            dirtyIndices.emplace_back(indexA);
                            dirtyIndices.emplace_back(indexB);
                            dirtyIndices.emplace_back(indexC);

                            if (i < step - 1)
                            {
                                const unsigned int hOff = (unsigned int)((hI * 0.5f) * (hI + 1));

                                const unsigned int indexD = start + hOff + nJ;

                                dirtyIndices.emplace_back(indexB);
                                dirtyIndices.emplace_back(indexD);
                                dirtyIndices.emplace_back(indexC);
                            }
                        }
                    }
                }

                char indexAB;
                char indexAC;
                char indexBC;

                const unsigned int edgeAB = GetEdge(dirtyEdges, face.Index[FaceIndex_3Point_AB], face.Index[FaceIndex_3Point_BA], &indexAB);
                const unsigned int edgeAC = GetEdge(dirtyEdges, face.Index[FaceIndex_3Point_AC], face.Index[FaceIndex_3Point_CA], &indexAC);
                const unsigned int edgeBC = GetEdge(dirtyEdges, face.Index[FaceIndex_3Point_BC], face.Index[FaceIndex_3Point_CB], &indexBC);

                dirtyEdges[edgeAB].SetCount[indexAB] = size;
                dirtyEdges[edgeAC].SetCount[indexAC] = size;
                dirtyEdges[edgeBC].SetCount[indexBC] = size;
                dirtyEdges[edgeAB].Set[indexAB] = new unsigned int[size];
                dirtyEdges[edgeAC].Set[indexAC] = new unsigned int[size];
                dirtyEdges[edgeBC].Set[indexBC] = new unsigned int[size];

                const unsigned int fOff = (unsigned int)((step * 0.5f) * (step + 1));
                const unsigned int nIndex = start + fOff;

                for (int i = 0; i < size; ++i)
                {
                    const unsigned int lOff = (unsigned int)((i * 0.5f) * (i + 1));        
                    const unsigned int lIndex = start + lOff;            

                    dirtyEdges[edgeAB].Set[indexAB][i] = lIndex;
                    dirtyEdges[edgeAC].Set[indexAC][i] = lIndex + i;
                    dirtyEdges[edgeBC].Set[indexBC][i] = nIndex + i;
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

                const unsigned int start = (unsigned int)dirtyVertices.size();

                const unsigned int xSize = xStep + 1;
                const unsigned int ySize = yStep + 1;

                for (int i = 0; i <= xStep; ++i)
                {
                    const int nI = i + 1;

                    const float iStep = (float)i / xStep;
                    const float bIStep = (float)(i - 1) / xStep;
                    const float fIStep = (float)(i + 1) / xStep;

                    const glm::vec3 pointAB = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_3Point_BA], 2.0f, iStep);
                    const glm::vec3 bPointAB = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_3Point_BA], 2.0f, bIStep);
                    const glm::vec3 fPointAB = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_3Point_BA], 2.0f, fIStep);

                    const glm::vec3 pointCD = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], 2.0f, iStep);
                    const glm::vec3 bPointCD = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], 2.0f, bIStep);
                    const glm::vec3 fPointCD = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], 2.0f, fIStep);

                    const glm::vec2 pointABUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], iStep);
                    const glm::vec2 pointCDUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], iStep);

                    glm::vec4 pointABBn;
                    glm::vec4 pointCDBn;
                    glm::vec4 pointABW;
                    glm::vec4 pointCDW;

                    GetBoneData(nodes[FaceIndex_4Point_AB], nodes[FaceIndex_4Point_BA], iStep, &pointABW, &pointABBn, boneCount, idMap);
                    GetBoneData(nodes[FaceIndex_4Point_CD], nodes[FaceIndex_4Point_DC], iStep, &pointCDW, &pointCDBn, boneCount, idMap);

                    const float bdHA = glm::mix(0.0f, iStep, iStep);
                    const float bdHB = glm::mix(iStep, 1.0f, iStep);
                    const float bdHM = glm::mix(bdHA, bdHB, iStep);
                    const float bdHMI = 1.0f - bdHM;

                    for (int j = 0; j <= yStep; ++j)
                    {
                        const int nJ = j + 1;

                        const float jStep = (float)j / yStep;
                        const float bJStep = (float)(j - 1) / yStep;
                        const float fJStep = (float)(j + 1) / yStep;

                        const glm::vec3 pointAC = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], 2.0f, jStep);
                        const glm::vec3 bPointAC = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], 2.0f, bJStep);
                        const glm::vec3 fPointAC = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], 2.0f, fJStep);

                        const glm::vec3 pointBD = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], 2.0f, jStep);
                        const glm::vec3 bPointBD = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], 2.0f, bJStep);
                        const glm::vec3 fPointBD = BezierCurveNode3::GetPointScaled(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], 2.0f, fJStep);

                        const glm::vec2 pointACUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], jStep);
                        const glm::vec2 pointBDUV = BezierCurveNode3::GetUVLerp(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], jStep);

                        const float bdVA = glm::mix(0.0f, jStep, jStep);
                        const float bdVB = glm::mix(jStep, 1.0f, jStep);
                        const float bdVM = glm::mix(bdVA, bdVB, jStep);
                        const float bdVMI = 1.0f - bdVM;

                        const glm::vec3 LM = glm::mix(pointAB, pointCD, jStep);
                        const glm::vec3 LF = glm::mix(pointAB, pointCD, fJStep);
                        const glm::vec3 LB = glm::mix(pointAB, pointCD, bJStep);

                        const glm::vec3 TM = glm::mix(pointAC, pointBD, iStep);
                        const glm::vec3 TF = glm::mix(pointAC, pointBD, fIStep);
                        const glm::vec3 TB = glm::mix(pointAC, pointBD, bIStep);

                        const glm::vec3 point = (LM + TM) * 0.5f;
                        const glm::vec3 pointR = (LF + TM) * 0.5f;
                        const glm::vec3 pointL = (LB + TM) * 0.5f;
                        const glm::vec3 pointT = (LM + TF) * 0.5f;
                        const glm::vec3 pointB = (LM + TB) * 0.5f;

                        const glm::vec3 vAA = pointR - point;
                        const glm::vec3 vAB = pointT - point;
                        const glm::vec3 vBA = pointL - point;
                        const glm::vec3 vBB = pointB - point;

                        const glm::vec2 hUV = glm::mix(pointAB, pointCD, jStep);
                        const glm::vec2 vUV = glm::mix(pointAC, pointBD, iStep);

                        const glm::vec2 uv = (hUV + vUV) * 0.5f;

                        const glm::vec3 normal = -glm::normalize(glm::cross(vAB, vAA) + glm::cross(vBB, vBA));

                        glm::vec4 pointACBn;
                        glm::vec4 pointBDBn;
                        glm::vec4 pointACW;
                        glm::vec4 pointBDW;

                        GetBoneData(nodes[FaceIndex_4Point_AC], nodes[FaceIndex_4Point_CA], jStep, &pointACW, &pointACBn, boneCount, idMap);
                        GetBoneData(nodes[FaceIndex_4Point_BD], nodes[FaceIndex_4Point_DB], jStep, &pointBDW, &pointBDBn, boneCount, idMap);

                        glm::vec4 hBn;
                        glm::vec4 hW;
                        glm::vec4 vBn;
                        glm::vec4 vW;

                        BlendBoneDataNL(pointABBn, pointABW, pointCDBn, pointCDW, jStep, &hBn, &hW);
                        BlendBoneDataNL(pointACBn, pointACW, pointBDBn, pointBDW, iStep, &vBn, &vW);

                        glm::vec4 bone;
                        glm::vec4 weight;

                        BlendBoneData(hBn, hW, vBn, vW, 0.5f, &bone, &weight);

                        for (int i = 0; i < 4; ++i)
                        {
                            if (bone[i] < 0)
                            {
                                bone[i] = 0.0f;
                                weight[i] = 0.0f;
                            }
                        }

                        const glm::vec4 bodyWA = glm::vec4((bdHM + bdVM) * 0.25f, (bdHMI + bdVM) * 0.25f, (bdHM + bdVMI) * 0.25f, (bdHMI + bdVMI) * 0.25f);

                        dirtyVertices.emplace_back(Vertex{ glm::vec4(point, 1.0f), normal, uv, bone, weight, bodyI, bodyWA });

                        if (nI < xSize && nJ < ySize)
                        {
                            const unsigned int indexA = start + (i * ySize) + j;
                            const unsigned int indexB = start + (i * ySize) + nJ;
                            const unsigned int indexC = start + (nI * ySize) + j;
                            const unsigned int indexD = start + (nI * ySize) + nJ;

                            dirtyIndices.emplace_back(indexA);
                            dirtyIndices.emplace_back(indexC);
                            dirtyIndices.emplace_back(indexB);

                            dirtyIndices.emplace_back(indexB);
                            dirtyIndices.emplace_back(indexC);
                            dirtyIndices.emplace_back(indexD);
                        }
                    }
                }

                char indexAB;
                char indexCD;
                char indexAC;
                char indexBD;

                const unsigned int edgeAB = GetEdge(dirtyEdges, face.Index[FaceIndex_4Point_AB], face.Index[FaceIndex_4Point_BA], &indexAB);
                const unsigned int edgeCD = GetEdge(dirtyEdges, face.Index[FaceIndex_4Point_CD], face.Index[FaceIndex_4Point_DC], &indexCD);
                const unsigned int edgeAC = GetEdge(dirtyEdges, face.Index[FaceIndex_4Point_AC], face.Index[FaceIndex_4Point_CA], &indexAC);
                const unsigned int edgeBD = GetEdge(dirtyEdges, face.Index[FaceIndex_4Point_BD], face.Index[FaceIndex_4Point_DB], &indexBD);

                dirtyEdges[edgeAB].SetCount[indexAB] = xSize;
                dirtyEdges[edgeAB].Set[indexAB] = new unsigned int[xSize];
                dirtyEdges[edgeCD].SetCount[indexCD] = xSize;
                dirtyEdges[edgeCD].Set[indexCD] = new unsigned int[xSize];

                for (int i = 0; i < xSize; ++i)
                {
                    const unsigned int xIndex = i * ySize;
                    const unsigned int index = start + xIndex;
                    dirtyEdges[edgeAB].Set[indexAB][i] = index;
                    dirtyEdges[edgeCD].Set[indexCD][i] = index + yStep;
                }

                dirtyEdges[edgeAC].SetCount[indexAC] = ySize;
                dirtyEdges[edgeAC].Set[indexAC] = new unsigned int[ySize];
                dirtyEdges[edgeBD].SetCount[indexBD] = ySize;
                dirtyEdges[edgeBD].Set[indexBD] = new unsigned int[ySize];

                const unsigned int xEnd = ySize * xStep;
                for (int i = 0; i < ySize; ++i)
                {
                    const unsigned int index = start + i;
                    dirtyEdges[edgeAC].Set[indexAC][i] = index;
                    dirtyEdges[edgeBD].Set[indexBD][i] = index + xEnd;
                }

                break;
            }
            }
        }

        std::unordered_map<unsigned int, unsigned int> indexMap;
        for (auto iter = dirtyEdges.begin(); iter != dirtyEdges.end(); ++iter)
        {
            const unsigned int setACount = iter->SetCount[0];
            const unsigned int setBCount = iter->SetCount[1];
            char bufferIndexA;
            char bufferIndexB;
            if (setACount > setBCount)
            {
                bufferIndexA = 0;
                bufferIndexB = 1;
            }
            else
            {
                bufferIndexA = 1;
                bufferIndexB = 0;
            }

            for (unsigned int i = 0; i < iter->SetCount[bufferIndexA]; ++i)
            {
                unsigned int setIndexA = iter->Set[bufferIndexA][i];
                const Vertex& vertA = dirtyVertices[setIndexA];

                float min = infinity;
                unsigned int index = 0;

                for (unsigned int j = 0; j < iter->SetCount[bufferIndexB]; ++j)
                {
                    const unsigned int setIndexB = iter->Set[bufferIndexB][j];
                    const Vertex& vertB = dirtyVertices[setIndexB];

                    const glm::vec3 diff = (vertB.Position - vertA.Position).xyz();
                    const float lenSqr = glm::dot(diff, diff);
                    if (lenSqr < min)
                    {
                        min = lenSqr;
                        index = setIndexB;
                    }
                }

                indexMap.emplace(setIndexA, index);
            }
        }

        *a_vertexCount = (unsigned int)dirtyVertices.size();
        *a_vertices = new Vertex[*a_vertexCount];
        for (unsigned int i = 0; i < *a_vertexCount; ++i)
        {
            (*a_vertices)[i] = dirtyVertices[i];
        }

        *a_indexCount = (unsigned int)dirtyIndices.size();
        *a_indices = new unsigned int[*a_indexCount];
        for (unsigned int i = 0; i < *a_indexCount; ++i)
        {
            unsigned int index = dirtyIndices[i];
            
            auto iter = indexMap.find(index);
            if (iter != indexMap.end())
            {
                index = iter->second;
            }

            (*a_indices)[i] = index;
        }

        for (auto iter = dirtyEdges.begin(); iter != dirtyEdges.end(); ++iter)
        {
            for (int i = 0; i < 2; ++i)
            {
                delete[] iter->Set[i];
            }
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
