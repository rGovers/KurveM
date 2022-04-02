#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"
#include "CurveModel.h"

class Editor;
class Workspace;

class DeleteNodeAction : public Action
{
private:
    Workspace*    m_workspace;
    Editor*       m_editor;
  
    CurveModel*   m_curveModel;

    unsigned int  m_nodeCount;
    Node3Cluster* m_nodes;
    unsigned int  m_faceCount;
    CurveFace*    m_faces;

    bool          m_own;

    unsigned int  m_oldNodeCount;
    Node3Cluster* m_oldNodes;
    unsigned int  m_oldFaceCount;
    CurveFace*    m_oldFaces;
protected:

public:
    DeleteNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel);
    virtual ~DeleteNodeAction();

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};