#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "CurveModel.h"

class Workspace;

class InsertFaceAction : public Action
{
private:
    Workspace*    m_workspace;

    CurveModel*   m_curveModel;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndices;
    unsigned int  m_faceIndex;

    void Wind3Points(const Node3Cluster* a_nodes, unsigned int* a_a, unsigned int* a_b, unsigned int* a_c) const;
    void Wind4Points(const Node3Cluster* a_nodes, unsigned int* a_a, unsigned int* a_b, unsigned int* a_c, unsigned int* a_d) const;

    bool GetIndex(const CurveFace& a_face, e_FaceIndex a_faceIndexA, e_FaceIndex a_faceIndexB, unsigned int a_indexA, unsigned int a_indexB, unsigned int* a_out) const;
    unsigned int PushNode(Node3Cluster* a_cluster, unsigned int a_startIndex, unsigned int a_endIndex, const CurveFace* a_faces, unsigned int a_faceCount) const;
    void PopNode(Node3Cluster* a_cluster, unsigned int a_index) const;

protected:

public:
    InsertFaceAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel);
    virtual ~InsertFaceAction();

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};