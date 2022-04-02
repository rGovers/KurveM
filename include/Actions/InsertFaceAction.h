#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class CurveModel;
class Workspace;

class InsertFaceAction : public Action
{
private:
    Workspace*    m_workspace;

    CurveModel*   m_curveModel;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndices;
    unsigned int  m_faceIndex;

protected:

public:
    InsertFaceAction(Workspace* a_workspace, unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel);
    virtual ~InsertFaceAction();

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};