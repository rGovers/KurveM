#pragma once

#include "BezierCurveNode3.h"

#include <vector>

class Model;

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

struct Node3Cluster
{
    std::vector<BezierCurveNode3> Nodes;

    Node3Cluster() {}
    Node3Cluster(const Node3Cluster& a_other) 
    {
        Nodes = a_other.Nodes;
    }
    Node3Cluster(const BezierCurveNode3& a_node)
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
    int           m_steps;

    unsigned int  m_nodeCount;
    Node3Cluster* m_nodes;
    unsigned int  m_faceCount;
    CurveFace*    m_faces;
    
    Model*        m_displayModel;

protected:

public:
    CurveModel();
    ~CurveModel();

    inline Model* GetDisplayModel() const
    {
        return m_displayModel;
    }

    inline int GetSteps() const
    {
        return m_steps;
    }
    void SetSteps(int a_value)
    {
        m_steps = a_value;
    }

    inline unsigned int GetFaceCount() const
    {
        return m_faceCount;
    }
    inline CurveFace* GetFaces() const
    {
        return m_faces;
    }

    inline Node3Cluster* GetNodes() const
    {
        return m_nodes;
    }

    void SetModelData(Node3Cluster* a_nodes, unsigned int a_nodeCount, CurveFace* a_faces, unsigned int a_faceCount);
    void Triangulate();
};