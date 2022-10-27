#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class CurveModel;
class Editor;
class Workspace;

struct CurveFace;
struct CurveNodeCluster;

class DeleteCurveNodeAction : public Action
{
private:
    Workspace*        m_workspace;
    Editor*           m_editor;
  
    CurveModel*       m_curveModel;

    unsigned int      m_nodeCount;
    CurveNodeCluster* m_nodes;
    unsigned int      m_faceCount;
    CurveFace*        m_faces;

    bool              m_own;

    unsigned int      m_oldNodeCount;
    CurveNodeCluster* m_oldNodes;
    unsigned int      m_oldFaceCount;
    CurveFace*        m_oldFaces;
protected:

public:
    DeleteCurveNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel);
    virtual ~DeleteCurveNodeAction();

    virtual e_ActionType GetActionType() const;

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};