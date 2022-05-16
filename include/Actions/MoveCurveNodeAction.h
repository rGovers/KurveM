#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "EditorControls/Editor.h"

class CurveModel;
class Workspace;

class MoveCurveNodeAction : public Action
{
private:
    Workspace*     m_workspace;

    CurveModel*    m_curveModel;

    unsigned int   m_nodeCount;
    unsigned int*  m_nodeIndices;
    unsigned int** m_mirroredIndices;

    glm::vec3*     m_oldPos;
    glm::vec3      m_startPos;
    glm::vec3      m_endPos;
 
    glm::vec3      m_axis;
 
    e_MirrorMode   m_mirrorMode;

    glm::vec3 GetMirrorMultiplier(e_MirrorMode a_mode) const;

protected:

public:
    MoveCurveNodeAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode);
    virtual ~MoveCurveNodeAction();

    inline void SetPosition(const glm::vec3& a_pos)
    {
        m_endPos = a_pos;
    }

    inline e_MirrorMode GetMirrorMode() const
    {
        return m_mirrorMode;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};