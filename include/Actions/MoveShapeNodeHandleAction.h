#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "EditorControls/Editor.h"

class PathModel;
class Workspace;

class MoveShapeNodeHandleAction : public Action
{
private:
    Workspace*     m_workspace;
    PathModel*     m_pathModel;
  
    unsigned int   m_nodeIndex;
    unsigned int*  m_mirroredIndex;
    unsigned char  m_clusterIndex;
    unsigned char* m_mirroredClusterIndex;

    glm::vec2      m_startPos;
    glm::vec2      m_startCursorPos;
    glm::vec2      m_endCursorPos;

    glm::vec2 GetMirrorMultiplier(e_MirrorMode a_mode) const;

protected:

public:
    MoveShapeNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, unsigned char a_clusterIndex, PathModel* a_pathModel, const glm::vec2& a_cursorPos, e_MirrorMode a_mirrorMode);
    virtual ~MoveShapeNodeHandleAction();

    inline void SetCursorPos(const glm::vec2& a_value)
    {
        m_endCursorPos = a_value;
    }

    virtual e_ActionType GetActionType() const;

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};