#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class PathModel;
class Workspace;

class MovePathNodeHandleAction : public Action
{
private:
    Workspace*   m_workspace;
    PathModel*   m_pathModel;

    unsigned int m_nodeIndex;

    glm::vec3    m_startPos;
    glm::vec2    m_startCursorPos;
    glm::vec2    m_endCursorPos;
     
    glm::vec3    m_xAxis;
    glm::vec3    m_yAxis;

protected:

public:
    MovePathNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, PathModel* a_pathModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis);
    virtual ~MovePathNodeHandleAction();

    inline void SetCursorPos(const glm::vec2& a_value)
    {
        m_endCursorPos = a_value;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};