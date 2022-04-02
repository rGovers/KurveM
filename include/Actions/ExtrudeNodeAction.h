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

    glm::vec3     m_startPos;
    glm::vec3     m_endPos;

    glm::vec3     m_axis;
    
protected:

public:
    ExtrudeNodeAction(Workspace* a_workspace, Editor* a_editor, unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis);
    virtual ~ExtrudeNodeAction();

    void SetPosition(const glm::vec3& a_position)
    {
        m_endPos = a_position;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};