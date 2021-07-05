#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class CurveModel;
class Editor;
class Workspace;

class ExtrudeNodeAction : public Action
{
private:
    Workspace*    m_workspace;
    Editor*       m_editor;

    CurveModel*   m_curveModel;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndices;

    unsigned int  m_startNodeIndex;
    unsigned int  m_startFaceIndex;

    glm::vec2     m_startCursorPos;
    glm::vec2     m_cursorPos;

    glm::vec3     m_xAxis;
    glm::vec3     m_yAxis;
    
protected:

public:
    ExtrudeNodeAction(Workspace* a_workspace, Editor* a_editor, unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis);
    ~ExtrudeNodeAction();

    void SetCursorPos(const glm::vec2& a_cursorPos)
    {
        m_cursorPos = a_cursorPos;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};