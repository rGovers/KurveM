#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class PathModel;
class Workspace;

class MovePathNodeAction : public Action
{
private:
    Workspace*    m_workspace;

    PathModel*    m_pathModel;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndices;

    glm::vec3*    m_oldPos;
    glm::vec3     m_startPos;
    glm::vec3     m_endPos;

    glm::vec3     m_axis;

protected:

public:
    MovePathNodeAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_pathModel, const glm::vec3& a_startPos, const glm::vec3& a_axis);
    virtual ~MovePathNodeAction();

    inline void SetPosition(const glm::vec3& a_value)
    {
        m_endPos = a_value;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};