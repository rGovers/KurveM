#pragma once

#include "Actions/Action.h"

class CurveModel;
class Workspace;

class FlipFaceAction : public Action
{
private:
    Workspace*    m_workspace;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndices;

    CurveModel*   m_model;

protected:

public:
    FlipFaceAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel);
    virtual ~FlipFaceAction();

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};