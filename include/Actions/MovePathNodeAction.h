#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "EditorControls/Editor.h"

class PathModel;
class Workspace;

class MovePathNodeAction : public Action
{
private:
    Workspace*     m_workspace;
 
    PathModel*     m_pathModel;
 
    unsigned int   m_nodeCount;
    unsigned int*  m_nodeIndices;
    unsigned int** m_mirrorIndices;

    glm::vec3*     m_oldPos;
    glm::vec3      m_startPos;
    glm::vec3      m_endPos;
 
    glm::vec3      m_axis;

    glm::vec3 GetMirrorMultiplier(e_MirrorMode a_mode) const;

protected:

public:
    MovePathNodeAction(Workspace* a_workspzace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_pathModel, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode);
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