#pragma once

#include "BezierCurveNode3.h"
#include "EditorControls/Editor.h"

#include <list>
#include <unordered_map>
#include <vector>

struct Vertex;

class Model;
class Object;
class Workspace;

enum e_FaceMode
{
    FaceMode_Null,
    FaceMode_3Point,
    FaceMode_4Point
};

enum e_FaceIndex
{
    FaceIndex_3Point_AB = 0,
    FaceIndex_3Point_AC = 1,
    FaceIndex_3Point_BA = 2,
    FaceIndex_3Point_BC = 3,
    FaceIndex_3Point_CA = 4,
    FaceIndex_3Point_CB = 5,

    FaceIndex_4Point_AB = 0,
    FaceIndex_4Point_AC = 1,
    FaceIndex_4Point_BA = 2,
    FaceIndex_4Point_BD = 3,
    FaceIndex_4Point_CA = 4,
    FaceIndex_4Point_CD = 5,
    FaceIndex_4Point_DB = 6,
    FaceIndex_4Point_DC = 7
};

struct CurveNode
{
    unsigned int FaceCount;
    BezierCurveNode3 Node;

    CurveNode() 
    {
        Node = BezierCurveNode3(glm::vec3(0.0f), glm::vec3(std::numeric_limits<float>::infinity()));
        FaceCount = 0;
    }
    CurveNode(const CurveNode& a_other)
    {
        Node = a_other.Node;
        FaceCount = a_other.FaceCount;
    }
    CurveNode(const BezierCurveNode3& a_node)
    {
        Node = a_node;
        FaceCount = 0;
    }
};

struct CurveNodeCluster
{
    std::vector<CurveNode> Nodes;

    CurveNodeCluster() {}
    CurveNodeCluster(const CurveNodeCluster& a_other) 
    {
        Nodes = a_other.Nodes;
    }
    CurveNodeCluster(const BezierCurveNode3& a_node)
    {
        CurveNode group; 
        group.Node = a_node;
        group.FaceCount = 0;

        Nodes.emplace_back(group);
    }
    CurveNodeCluster(const CurveNode& a_node)
    {
        Nodes.emplace_back(a_node);
    }
};

struct CurveFace
{
    e_FaceMode FaceMode;

    unsigned int Index[8];
    unsigned int ClusterIndex[8];
};

struct CurveEdge
{
    unsigned int IndexL;
    unsigned int IndexH;

    unsigned int SetCount[2];
    unsigned int* Set[2];
};

class CurveModel
{
private:
    friend class CurveModelSerializer;

    Workspace*        m_workspace;

    long long         m_armature;

    int               m_steps;

    bool              m_stepAdjust;

    unsigned int      m_nodeCount;
    CurveNodeCluster* m_nodes;
    unsigned int      m_faceCount;
    CurveFace*        m_faces;
    
    Model*            m_displayModel;

    float GetNodeDist(const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB) const;

    float BlendWeight(float a_weightA, float a_weightB, float a_lerp) const;

    void GetBoneData(const BezierCurveNode3& a_nodeA, const BezierCurveNode3& a_nodeB, float a_lerp, glm::vec4* a_weights, glm::vec4* a_bones, unsigned int a_boneCount, const std::unordered_map<long long, unsigned int>& a_idMap) const;
    void BlendBoneDataNL(const glm::vec4& a_bonesA, const glm::vec4& a_weightsA, const glm::vec4& a_bonesB, const glm::vec4& a_weightsB, float a_lerp, glm::vec4* a_bones, glm::vec4* a_weights) const;
    void BlendBoneData(const glm::vec4& a_bonesA, const glm::vec4& a_weightsA, const glm::vec4& a_bonesB, const glm::vec4& a_weightsB, float a_lerp, glm::vec4* a_bones, glm::vec4* a_weights) const;
    glm::vec2 BlendUV(const glm::vec2& a_start, const glm::vec2& a_end, float a_lerp) const;

    unsigned int GetEdge(std::vector<CurveEdge>& a_edge, unsigned int a_indexA, unsigned int a_indexB, char* a_index) const;

    glm::vec3 GetMirrorMultiplier(e_MirrorMode a_mode) const;

protected:

public:
    CurveModel(Workspace* a_workspace);
    ~CurveModel();

    inline Model* GetDisplayModel() const
    {
        return m_displayModel;
    }

    inline int GetSteps() const
    {
        return m_steps;
    }
    inline void SetSteps(int a_value)
    {
        m_steps = a_value;
    }

    inline unsigned int GetFaceCount() const
    {
        return m_faceCount;
    }
    inline CurveFace GetFace(unsigned int a_index) const
    {
        return m_faces[a_index];
    }
    inline CurveFace* GetFaces() const
    {
        return m_faces;
    }

    inline unsigned int GetNodeCount() const
    {
        return m_nodeCount;
    }
    inline CurveNodeCluster GetNode(unsigned int a_index) const
    {
        return m_nodes[a_index];
    }
    inline CurveNodeCluster* GetNodes() const
    {
        return m_nodes;
    }

    inline void SetStepAdjust(bool a_value)
    {
        m_stepAdjust = a_value;
    }
    inline bool IsStepAdjusted() const
    {
        return m_stepAdjust;
    }

    void SetArmature(long long a_id);
    void SetArmature(const Object* a_armature);
    Object* GetArmature() const;
    inline long long GetArmatureID() const
    {
        return m_armature;
    }
    std::list<Object*> GetArmatureNodes() const;
    unsigned int GetArmatureNodeCount() const;

    unsigned int* GetMirroredIndices(unsigned int a_index, e_MirrorMode a_mirrorMode) const;
    void GetMirroredHandles(unsigned int a_index, unsigned int a_nodeIndex, e_MirrorMode a_mode, unsigned int** a_outIndices, unsigned int** a_outNodeIndices) const;

    unsigned int Get3PointFaceIndex(unsigned int a_indexA, unsigned int a_indexB, unsigned int a_indexC) const;
    unsigned int Get3PointFaceIndex(const unsigned int a_indices[6]) const;

    unsigned int Get4PointFaceIndex(unsigned int a_indexA, unsigned int a_indexB, unsigned int a_indexC, unsigned int a_indexD) const;
    unsigned int Get4PointFaceIndex(const unsigned int a_indices[8]) const;

    void EmplaceFace(const CurveFace& a_face);
    void EmplaceFaces(const CurveFace* a_faces, unsigned int a_count);

    void EmplaceNode(unsigned int a_index, const CurveNodeCluster& a_node);
    void EmplaceNode(const CurveNodeCluster& a_node);
    void EmplaceNodes(const CurveNodeCluster* a_nodes, unsigned int a_count);

    void DestroyFace(unsigned int a_index);
    void DestroyFaces(unsigned int a_start, unsigned int a_end);

    void DestroyNode(unsigned int a_index);
    void DestroyNodes(unsigned int a_start, unsigned int a_end);

    void SetModelData(const CurveNodeCluster* a_nodes, unsigned int a_nodeCount, const CurveFace* a_faces, unsigned int a_faceCount);
    void PassModelData(CurveNodeCluster* a_nodes, unsigned int a_nodeCount, CurveFace* a_faces, unsigned int a_faceCount);
    void Triangulate();

    void GetModelData(bool a_smartStep, int a_steps, unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const;

    void PreTriangulate(unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const;
    void PostTriangulate(const unsigned int* a_indices, unsigned int a_indexCount, const Vertex* a_vertices, unsigned int a_vertexCount);
};