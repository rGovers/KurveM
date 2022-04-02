#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class PathModel;
class Workspace;

class MoveShapeNodeHandleAction : public Action
{
private:
    Workspace*   m_workspace;
    PathModel*   m_pathModel;

    unsigned int m_nodeIndex;

    glm::vec2    m_startPos;
    glm::vec2    m_startCursorPos;
    glm::vec2    m_endCursorPos;

protected:

public:
    MoveShapeNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, PathModel* a_pathModel, const glm::vec2& a_cursorPos);
    virtual ~MoveShapeNodeHandleAction();

    inline void SetCursorPos(const glm::vec2& a_value)
    {
        m_endCursorPos = a_value;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};