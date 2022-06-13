#include "PathModel.h"

#include "Model.h"
#include "Workspace.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

PathModel::PathModel(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_model = nullptr;

    m_pathNodes = nullptr;
    m_pathLines = nullptr;

    m_shapeNodes = nullptr;
    m_shapeLines = nullptr;

    m_armature = -1;

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

glm::vec3 PathModel::GetMirrorMultiplier(e_MirrorMode a_mode) const
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

void PathModel::SetArmature(long long a_value)
{
    const Object* obj = m_workspace->GetObject(a_value);

    SetArmature(obj);
}
void PathModel::SetArmature(const Object* a_value)
{
    m_armature = -1;

    if (a_value != nullptr && a_value->GetObjectType() == ObjectType_Armature)
    {
        m_armature = a_value->GetID();
    }
}
Object* PathModel::GetArmature() const
{
    return m_workspace->GetObject(m_armature);
}

void PathModel::GetArmatureNodes(std::list<Object*>* a_list, Object* a_object) const
{
    if (a_object != nullptr && a_object->GetObjectType() == ObjectType_ArmatureNode)
    {
        a_list->emplace_back(a_object);

        const std::list<Object*> children = a_object->GetChildren();
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            GetArmatureNodes(a_list, *iter);
        }
    }
}

std::list<Object*> PathModel::GetArmatureNodes() const
{
    std::list<Object*> nodes;

    const Object* obj = m_workspace->GetObject(m_armature);
    if (obj != nullptr && obj->GetObjectType() == ObjectType_Armature)
    {
        const std::list<Object*> children = obj->GetChildren();
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            GetArmatureNodes(&nodes, *iter);
        }
    }

    return nodes;
}
unsigned int PathModel::GetArmatureNodeCount() const
{
    return GetArmatureNodes().size();
}

unsigned int* PathModel::GetMirroredPathIndices(unsigned int a_index, e_MirrorMode a_mode) const
{
    unsigned int* indices = new unsigned int[7];

    const bool xMode = (a_mode & MirrorMode_X) != 0;
    const bool yMode = (a_mode & MirrorMode_Y) != 0;
    const bool zMode = (a_mode & MirrorMode_Z) != 0;

    const glm::vec3 pos = m_pathNodes[a_index].Nodes[0].Node.GetPosition();

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

        for (unsigned int j = 0; j < m_pathNodeCount; ++j)
        {
            if (j == a_index)
            {
                continue;
            }

            const glm::vec3 pos = m_pathNodes[j].Nodes[0].Node.GetPosition();

            if (glm::length(pos - invPos) <= 0.0001f)
            {
                indices[i] = j;

                break;
            }
        }
    }

    return indices;
}
void PathModel::GetMirroredPathHandle(unsigned int a_index, unsigned char a_nodeIndex, e_MirrorMode a_mode, unsigned int** a_outIndex, unsigned char** a_outNodeIndex) const
{
    *a_outIndex = new unsigned int[7];
    *a_outNodeIndex = new unsigned char[7];

    const bool xMode = (a_mode & MirrorMode_X) != 0;
    const bool yMode = (a_mode & MirrorMode_Y) != 0;
    const bool zMode = (a_mode & MirrorMode_Z) != 0;

    const BezierCurveNode3& curve = m_pathNodes[a_index].Nodes[a_nodeIndex].Node;
    const glm::vec3 pos = curve.GetPosition();
    const glm::vec3 hPos = curve.GetHandlePosition();

    for (int i = 0; i < 7; ++i)
    {
        (*a_outIndex)[i] = -1;
        (*a_outNodeIndex)[i] = -1;

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

        for (unsigned int j = 0; j < m_pathNodeCount; ++j)
        {
            const PathNodeCluster& c = m_pathNodes[j];

            const glm::vec3 pos = c.Nodes[0].Node.GetPosition();
            if (glm::length(pos - invPos) <= 0.0001f)
            {
                const unsigned char size = (unsigned char)c.Nodes.size();

                for (unsigned char k = 0; k < size; ++k)
                {
                    const glm::vec3 hPos = c.Nodes[k].Node.GetHandlePosition();
                    if (glm::length(hPos - invHPos) <= 0.001f)
                    {
                        (*a_outIndex)[i] = j;
                        (*a_outNodeIndex)[i] = k;

                        goto NextIter;
                    }
                }
            }
        }

NextIter:;
    }
}

unsigned int* PathModel::GetMirroredShapeIndices(unsigned int a_index, e_MirrorMode a_mode) const
{
    unsigned int* indices = new unsigned int[3];

    const bool xMode = (a_mode & MirrorMode_X) != 0;
    const bool yMode = (a_mode & MirrorMode_Y) != 0;

    const glm::vec2 pos = m_shapeNodes[a_index].Nodes[0].GetPosition();

    for (int i = 0; i < 3; ++i)
    {
        indices[i] = -1;

        const e_MirrorMode mode = (e_MirrorMode)(i + 1);

        if ((mode & MirrorMode_X && !xMode) ||
            (mode & MirrorMode_Y && !yMode))
        {
            continue;
        }

        const glm::vec2 mul = GetMirrorMultiplier(mode).xy();
        const glm::vec2 invPos = pos * mul;

        for (unsigned int j = 0; j < m_shapeNodeCount; ++j)
        {
            if (j == a_index)
            {
                continue;
            }

            const glm::vec2 pos = m_shapeNodes[j].Nodes[0].GetPosition();

            if (glm::length(pos - invPos) <= 0.0001f)
            {
                indices[i] = j;

                break;
            }
        }
    }

    return indices;
}
void PathModel::GetMirroredShapeHandle(unsigned int a_index, unsigned char a_nodeIndex, e_MirrorMode a_mode, unsigned int** a_outIndex, unsigned char** a_outNodeIndex) const
{
    *a_outIndex = new unsigned int[3];
    *a_outNodeIndex = new unsigned char[3];

    const bool xMode = (a_mode & MirrorMode_X) != 0;
    const bool yMode = (a_mode & MirrorMode_Y) != 0;

    const BezierCurveNode2& node = m_shapeNodes[a_index].Nodes[a_nodeIndex];

    const glm::vec2 pos = node.GetPosition();
    const glm::vec2 hPos = node.GetHandlePosition();

    for (int i = 0; i < 3; ++i)
    {
        (*a_outIndex)[i] = -1;
        (*a_outNodeIndex)[i] = -1;

        const e_MirrorMode mode = (e_MirrorMode)(i + 1);

        if ((mode & MirrorMode_X && !xMode) ||
            (mode & MirrorMode_Y && !yMode))
        {
            continue;
        }

        const glm::vec2 mul = GetMirrorMultiplier(mode).xy();
        const glm::vec2 invPos = pos * mul;
        const glm::vec2 invHPos = hPos * mul;

        for (unsigned int j = 0; j < m_shapeNodeCount; ++j)
        {
            const ShapeNodeCluster& c = m_shapeNodes[j];

            const glm::vec2 pos = c.Nodes[0].GetPosition();

            if (glm::length(pos - invPos) <= 0.0001f)
            {
                const unsigned char size = (unsigned char)c.Nodes.size();

                for (unsigned char k = 0; k < size; ++k)
                {
                    const glm::vec2 hPos = c.Nodes[k].GetHandlePosition();
                    if (glm::length(hPos - invHPos) <= 0.0001f)
                    {
                        (*a_outIndex)[i] = j;
                        (*a_outNodeIndex)[i] = k;

                        goto NextIter;
                    }
                }
            }
        }

NextIter:;
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

void PathModel::GeneratePathStep(const PathLine& a_line, const std::vector<Vertex>& a_shapeVertices, float a_lerp, float a_nextLerp, const std::unordered_map<long long, unsigned int>& a_idMap, std::vector<Vertex>* a_vertices) const
{
    constexpr float infinity = std::numeric_limits<float>::infinity();
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();
    constexpr float pi = glm::pi<float>();
    constexpr float pi2 = pi * 2;

    const unsigned int indexA = a_line.Index[0];
    const unsigned int indexB = a_line.Index[1];

    const unsigned char clusterIndexA = a_line.ClusterIndex[0];
    const unsigned char clusterIndexB = a_line.ClusterIndex[1];

    const glm::vec4 bodyI = glm::vec4((float)indexA / m_pathNodeCount, (float)indexB / m_pathNodeCount, 0.0f, 0.0f);

    const PathNode& nodeA = m_pathNodes[indexA].Nodes[clusterIndexA];
    const PathNode& nodeB = m_pathNodes[indexB].Nodes[clusterIndexB];

    const PathNode& sNodeA = m_pathNodes[indexA].Nodes[0];
    const PathNode& sNodeB = m_pathNodes[indexB].Nodes[0];

    const glm::vec3 nDA = nodeA.Node.GetHandlePosition() - nodeA.Node.GetPosition();
    // const glm::vec3 nDB = nodeB.Node.GetHandlePosition() - nodeB.Node.GetPosition();

    const glm::vec3 nSDA = sNodeA.Node.GetHandlePosition() - sNodeA.Node.GetPosition();
    // const glm::vec3 nSDB = sNodeB.Node.GetHandlePosition() - sNodeB.Node.GetPosition();

    float sRA = -1.0f;
    if (glm::dot(nDA, nSDA) >= 0.0f)
    {
        sRA = 1.0f;
    }
    // float sRB = -1.0f;
    // if (glm::dot(nDB, nSDB) >= 0.0f)
    // {
    //     sRB = 1.0f;
    // }

    const glm::vec3 pPos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, a_lerp);
    const glm::vec3 nextPos = BezierCurveNode3::GetPoint(nodeA.Node, nodeB.Node, a_nextLerp);

    unsigned int boneCount;
    const BoneCluster* boneCluster = BezierCurveNode3::GetBonesLerp(nodeA.Node, nodeB.Node, a_lerp, &boneCount);

    glm::vec4 bones = glm::vec4(0.0f);
    glm::vec4 weights = glm::vec4(0.0f);

    for (unsigned int i = 0; i < boneCount; ++i)
    {
        const BoneCluster& bone = boneCluster[i];

        float min = infinity;
        int index = 0;

        for (int j = 0; j < 4; ++j)
        {
            const float weight = weights[j];

            if (weight < min)
            {
                min = weight;
                index = j;
            }
        }

        if (bone.Weight > min)
        {
            weights[i] = bone.Weight;
            bones[i] = a_idMap.at(bone.ID) / (float)boneCount;
        }
    }

    delete[] boneCluster;

    const glm::vec3 forward = glm::normalize(nextPos - pPos);

    const float bL = glm::mix(0.0f, a_lerp, a_lerp);
    const float bR = glm::mix(a_lerp, 1.0f, a_lerp);
    const float bM = glm::mix(bL, bR, a_lerp);

    const float rot = glm::mix(nodeA.Rotation * sRA, nodeB.Rotation * sRA, bM);
    const glm::vec2 scale = glm::mix(nodeA.Scale, nodeB.Scale, bM);

    const glm::vec4 bodyW = glm::vec4(1 - bM, bM, 0.0f, 0.0f);

    const glm::quat rotAxis = glm::angleAxis(rot, forward);

    for (auto iter = a_shapeVertices.begin(); iter != a_shapeVertices.end(); ++iter)
    {
        const Vertex& sVert = *iter;

        const glm::vec3 norm = rotAxis * sVert.Normal;

        const glm::vec2 scalePos = scale * sVert.Position.xz();
        const glm::vec3 rotPos = rotAxis * glm::vec3(scalePos.x, 0.0f, scalePos.y);

        const glm::vec3 pos = rotPos + pPos;

        a_vertices->emplace_back(Vertex{ glm::vec4(pos, 1.0f), -norm, glm::vec2(0.0f), bones, weights, bodyI, bodyW });
        a_vertices->emplace_back(Vertex{ glm::vec4(pos, 1.0f), norm, glm::vec2(0.0f), bones, weights, bodyI, bodyW });
    }
}

void PathModel::GetModelData(int a_shapeSteps, int a_pathSteps, unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();
    constexpr float infinity = std::numeric_limits<float>::infinity();

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

    const unsigned int shapeVertexCount = (unsigned int)shapeVertices.size();

    std::unordered_map<long long, unsigned int> idMap;
    const std::list<Object*> armNodes = GetArmatureNodes();

    const unsigned int boneCount = (unsigned int)armNodes.size();
    index = 0;
    for (auto iter = armNodes.begin(); iter != armNodes.end(); ++iter)
    {
        idMap.emplace((*iter)->GetID(), index++);
    }

    std::unordered_map<unsigned int, unsigned int*> indexMap;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    const unsigned int shapeLines = shapeIndexCount / 2;
    const int iPathSteps = a_pathSteps - 1;
    const unsigned int indexStep = shapeVertexCount * 2;

    if (a_pathSteps <= 1)
    {
        for (unsigned int i = 0; i < m_pathLineCount; ++i)
        {
            const PathLine& line = m_pathLines[i];
            
            const unsigned int lIndexA = line.Index[0];
            const unsigned int lIndexB = line.Index[1];

            const auto iterA = indexMap.find(lIndexA);
            const auto iterB = indexMap.find(lIndexB);

            const bool foundA = iterA != indexMap.end();
            const bool foundB = iterB != indexMap.end();

            const unsigned int start = (unsigned int)vertices.size();

            if (foundA && foundB)
            {
                unsigned int* lIndA = iterA->second;
                unsigned int* lIndB = iterB->second;
                for (unsigned int i = 0; i < shapeLines; ++i)
                {
                    const unsigned int index = i * 2;

                    const unsigned int indexA = lIndA[index];
                    const unsigned int indexB = lIndA[index + 1];
                    const unsigned int indexC = lIndB[index];
                    const unsigned int indexD = lIndB[index + 1];

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);

                    indices.emplace_back(indexB);
                    indices.emplace_back(indexD);
                    indices.emplace_back(indexC);

                    const unsigned int invA = indexA + 1;
                    const unsigned int invB = indexB + 1;
                    const unsigned int invC = indexC + 1;
                    const unsigned int invD = indexD + 1;

                    indices.emplace_back(invA);
                    indices.emplace_back(invC);
                    indices.emplace_back(invB);

                    indices.emplace_back(invB);
                    indices.emplace_back(invC);
                    indices.emplace_back(invD);
                }
            }
            else if (foundA)
            {
                GeneratePathStep(line, shapeVertices, 1.0f, 1.0f + (1.0f / a_pathSteps), idMap, &vertices);

                unsigned int* idBuff = new unsigned int[shapeIndexCount];

                unsigned int* lIndA = iterA->second;
                for (unsigned int i = 0; i < shapeLines; ++i)
                {
                    const unsigned int index = i * 2;

                    const unsigned int indexA = lIndA[index];
                    const unsigned int indexB = lIndA[index + 1];
                    const unsigned int indexC = start + (shapeIndices[index] * 2);
                    const unsigned int indexD = start + (shapeIndices[index + 1] * 2);

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);

                    indices.emplace_back(indexB);
                    indices.emplace_back(indexD);
                    indices.emplace_back(indexC);

                    idBuff[index] = indexC;
                    idBuff[index + 1] = indexD;

                    const unsigned int invA = indexA + 1;
                    const unsigned int invB = indexB + 1;
                    const unsigned int invC = indexC + 1;
                    const unsigned int invD = indexD + 1;

                    indices.emplace_back(invA);
                    indices.emplace_back(invC);
                    indices.emplace_back(invB);

                    indices.emplace_back(invB);
                    indices.emplace_back(invC);
                    indices.emplace_back(invD);
                }

                indexMap.emplace(lIndexB, idBuff);
            }
            else if (foundB)
            {
                GeneratePathStep(line, shapeVertices, 0.0f, 1.0f / a_pathSteps, idMap, &vertices);

                unsigned int* idBuff = new unsigned int[shapeIndexCount];

                unsigned int* lIndB = iterB->second;
                for (unsigned int i = 0; i < shapeLines; ++i)
                {
                    const unsigned int index = i * 2;

                    const unsigned int indexA = start + (shapeIndices[index] * 2);
                    const unsigned int indexB = start + (shapeIndices[index + 1] * 2);
                    const unsigned int indexC = lIndB[index];
                    const unsigned int indexD = lIndB[index + 1];

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);

                    indices.emplace_back(indexB);
                    indices.emplace_back(indexD);
                    indices.emplace_back(indexC);

                    idBuff[index] = indexA;
                    idBuff[index + 1] = indexB;

                    const unsigned int invA = indexA + 1;
                    const unsigned int invB = indexB + 1;
                    const unsigned int invC = indexC + 1;
                    const unsigned int invD = indexD + 1;

                    indices.emplace_back(invA);
                    indices.emplace_back(invC);
                    indices.emplace_back(invB);

                    indices.emplace_back(invB);
                    indices.emplace_back(invC);
                    indices.emplace_back(invD);
                }

                indexMap.emplace(lIndexA, idBuff);
            }
            else
            {
                GeneratePathStep(line, shapeVertices, 0.0f, 1.0f / a_pathSteps, idMap, &vertices);
                GeneratePathStep(line, shapeVertices, 1.0f, 1.0f + (1.0f / a_pathSteps), idMap, &vertices);

                unsigned int* idBuffA = new unsigned int[shapeIndexCount];
                unsigned int* idBuffB = new unsigned int[shapeIndexCount];

                for (unsigned int i = 0; i < shapeLines; ++i)
                {
                    const unsigned int index = i * 2;

                    const unsigned int indexA = start + (shapeIndices[index] * 2);
                    const unsigned int indexB = start + (shapeIndices[index + 1] * 2);
                    const unsigned int indexC = indexA + indexStep;
                    const unsigned int indexD = indexB + indexStep;

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);

                    indices.emplace_back(indexB);
                    indices.emplace_back(indexD);
                    indices.emplace_back(indexC);

                    idBuffA[index] = indexA;
                    idBuffA[index + 1] = indexB;
                    idBuffB[index] = indexC;
                    idBuffB[index + 1] = indexD;

                    const unsigned int invA = indexA + 1;
                    const unsigned int invB = indexB + 1;
                    const unsigned int invC = indexC + 1;
                    const unsigned int invD = indexD + 1;

                    indices.emplace_back(invA);
                    indices.emplace_back(invC);
                    indices.emplace_back(invB);

                    indices.emplace_back(invB);
                    indices.emplace_back(invC);
                    indices.emplace_back(invD);
                }

                indexMap.emplace(lIndexA, idBuffA);
                indexMap.emplace(lIndexB, idBuffB);
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < m_pathLineCount; ++i)
        {
            const PathLine& line = m_pathLines[i];

            const unsigned int lIndexA = line.Index[0];
            const unsigned int lIndexB = line.Index[1];

            auto iter = indexMap.find(lIndexA);
            if (iter != indexMap.end())
            {
                const unsigned int start = (unsigned int)vertices.size();

                unsigned int* lInd = iter->second;
                for (unsigned int i = 0; i < shapeLines; ++i)
                {
                    const unsigned int index = i * 2;

                    const unsigned int indexA = lInd[index];
                    const unsigned int indexB = lInd[index + 1];
                    const unsigned int indexC = start + (shapeIndices[index] * 2);
                    const unsigned int indexD = start + (shapeIndices[index + 1] * 2);

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);

                    indices.emplace_back(indexB);
                    indices.emplace_back(indexD);
                    indices.emplace_back(indexC);

                    const unsigned int invA = indexA + 1;
                    const unsigned int invB = indexB + 1;
                    const unsigned int invC = indexC + 1;
                    const unsigned int invD = indexD + 1;

                    indices.emplace_back(invA);
                    indices.emplace_back(invC);
                    indices.emplace_back(invB);

                    indices.emplace_back(invB);
                    indices.emplace_back(invC);
                    indices.emplace_back(invD);
                }
            }
            else
            {
                const unsigned int start = (unsigned int)vertices.size();

                GeneratePathStep(line, shapeVertices, 0.0f, 1.0f / a_pathSteps, idMap, &vertices);

                unsigned int* idBuff = new unsigned int[shapeIndexCount];

                for (unsigned int i = 0; i < shapeLines; ++i)
                {
                    const unsigned int index = i * 2;

                    const unsigned int indexA = start + shapeIndices[index] * 2;
                    const unsigned int indexB = start + shapeIndices[index + 1] * 2;
                    const unsigned int indexC = indexA + indexStep;
                    const unsigned int indexD = indexB + indexStep;

                    idBuff[index] = indexA;
                    idBuff[index + 1] = indexB;

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);

                    indices.emplace_back(indexB);
                    indices.emplace_back(indexD);
                    indices.emplace_back(indexC);

                    const unsigned int invA = indexA + 1;
                    const unsigned int invB = indexB + 1;
                    const unsigned int invC = indexC + 1;
                    const unsigned int invD = indexD + 1;

                    indices.emplace_back(invA);
                    indices.emplace_back(invC);
                    indices.emplace_back(invB);

                    indices.emplace_back(invB);
                    indices.emplace_back(invC);
                    indices.emplace_back(invD);
                }

                indexMap.emplace(lIndexA, idBuff);
            }

            for (int i = 0; i < iPathSteps; ++i)
            {
                const unsigned int start = (unsigned int)vertices.size();

                GeneratePathStep(line, shapeVertices, (float)(i + 1) / a_pathSteps, (float)(i + 2) / a_pathSteps, idMap, &vertices);

                if (i < iPathSteps - 1)
                {
                    for (unsigned int j = 0; j < shapeLines; ++j)
                    {
                        const unsigned int index = j * 2;

                        const unsigned int indexA = start + shapeIndices[index] * 2;
                        const unsigned int indexB = start + shapeIndices[index + 1] * 2;
                        const unsigned int indexC = indexA + indexStep;
                        const unsigned int indexD = indexB + indexStep;

                        indices.emplace_back(indexA);
                        indices.emplace_back(indexB);
                        indices.emplace_back(indexC);

                        indices.emplace_back(indexB);
                        indices.emplace_back(indexD);
                        indices.emplace_back(indexC);

                        const unsigned int invA = indexA + 1;
                        const unsigned int invB = indexB + 1;
                        const unsigned int invC = indexC + 1;
                        const unsigned int invD = indexD + 1;

                        indices.emplace_back(invA);
                        indices.emplace_back(invC);
                        indices.emplace_back(invB);

                        indices.emplace_back(invB);
                        indices.emplace_back(invC);
                        indices.emplace_back(invD);
                    }
                }
            }

            iter = indexMap.find(lIndexB);
            if (iter != indexMap.end())
            {
                const unsigned int start = (unsigned int)vertices.size() - indexStep;

                unsigned int* lInd = iter->second;
                for (unsigned int i = 0; i < shapeLines; ++i)
                {
                    const unsigned int index = i * 2;

                    const unsigned int indexA = lInd[index];
                    const unsigned int indexB = lInd[index + 1];
                    const unsigned int indexC = start + (shapeIndices[index] * 2);
                    const unsigned int indexD = start + (shapeIndices[index + 1] * 2);

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);

                    indices.emplace_back(indexB);
                    indices.emplace_back(indexD);
                    indices.emplace_back(indexC);

                    const unsigned int invA = indexA + 1;
                    const unsigned int invB = indexB + 1;
                    const unsigned int invC = indexC + 1;
                    const unsigned int invD = indexD + 1;

                    indices.emplace_back(invA);
                    indices.emplace_back(invC);
                    indices.emplace_back(invB);

                    indices.emplace_back(invB);
                    indices.emplace_back(invC);
                    indices.emplace_back(invD);
                }
            }
            else
            {
                const unsigned int start = (unsigned int)vertices.size();

                GeneratePathStep(line, shapeVertices, 1.0f, 1.0f + (1.0f / a_pathSteps), idMap, &vertices);

                unsigned int* idBuff = new unsigned int[shapeIndexCount];

                for (unsigned int i = 0; i < shapeLines; ++i)
                {
                    const unsigned int index = i * 2;

                    const unsigned int indexA = start + shapeIndices[index] * 2;
                    const unsigned int indexB = start + shapeIndices[index + 1] * 2;
                    const unsigned int indexC = indexA - indexStep;
                    const unsigned int indexD = indexB - indexStep;

                    indices.emplace_back(indexA);
                    indices.emplace_back(indexC);
                    indices.emplace_back(indexB);

                    indices.emplace_back(indexB);
                    indices.emplace_back(indexC);
                    indices.emplace_back(indexD);

                    idBuff[index] = indexA;
                    idBuff[index + 1] = indexB;

                    const unsigned int invA = indexA + 1;
                    const unsigned int invB = indexB + 1;
                    const unsigned int invC = indexC + 1;
                    const unsigned int invD = indexD + 1;

                    indices.emplace_back(invA);
                    indices.emplace_back(invB);
                    indices.emplace_back(invC);

                    indices.emplace_back(invB);
                    indices.emplace_back(invD);
                    indices.emplace_back(invC);
                }

                indexMap.emplace(lIndexB, idBuff);
            }
        }
    }

    *a_vertexCount = (unsigned int)vertices.size();
    *a_vertices = new Vertex[*a_vertexCount];

    for (unsigned int i = 0; i < *a_vertexCount; ++i)
    {
        (*a_vertices)[i] = vertices[i];
    }

    for (auto iter = indexMap.begin(); iter != indexMap.end(); ++iter)
    {
        delete[] iter->second;
    }

    delete[] shapeIndices;

    *a_indexCount = (unsigned int)indices.size();
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
