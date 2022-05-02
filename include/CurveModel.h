#pragma once

#include "BezierCurveNode3.h"

#include <list>
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

    CurveNode() {}
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

class CurveModel
{
private:
    friend class CurveModelSerializer;

    Workspace*    m_workspace;

    long long     m_armature;

    int           m_steps;

    bool          m_stepAdjust;

    unsigned int  m_nodeCount;
    CurveNodeCluster* m_nodes;
    unsigned int  m_faceCount;
    CurveFace*    m_faces;
    
    Model*        m_displayModel;

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