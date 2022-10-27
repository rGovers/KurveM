#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class CurveModel;
class Workspace;

class MoveUVAction : public Action
{
private:
    Workspace*    m_workspace;

    CurveModel*   m_curveModel;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndices;
    
    glm::vec2*    m_oldPos;
    glm::vec2     m_startPos;
    glm::vec2     m_endPos;
    
    glm::vec2     m_axis;

protected:

public:
    MoveUVAction(Workspace* a_workspace, unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec2& a_startPos, const glm::vec2& a_axis);
    virtual ~MoveUVAction();

    void SetPosition(const glm::vec2& a_pos)
    {
        m_endPos = a_pos;
    }

    virtual e_ActionType GetActionType() const;

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};