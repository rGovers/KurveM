#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "EditorControls/Editor.h"

class CurveModel;
class Workspace;

class ExtrudeCurveNodeAction : public Action
{
private:
    Workspace*     m_workspace;
    Editor*        m_editor;
 
    CurveModel*    m_curveModel;
 
    unsigned int   m_nodeCount;
    unsigned int*  m_nodeIndices;
    unsigned int** m_mirrorIndices;

    unsigned int   m_startNodeIndex;
    unsigned int   m_startFaceIndex;
 
    glm::vec3      m_startPos;
    glm::vec3      m_endPos;
 
    glm::vec3      m_axis;
     
    e_MirrorMode   m_mirrorMode;

    glm::vec3 GetMirrorMultiplier(e_MirrorMode a_mode) const;

protected:

public:
    ExtrudeCurveNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode);
    virtual ~ExtrudeCurveNodeAction();

    inline void SetPosition(const glm::vec3& a_position)
    {
        m_endPos = a_position;
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