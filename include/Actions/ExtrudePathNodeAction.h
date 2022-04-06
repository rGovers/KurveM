#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Editor;
class PathModel;
class Workspace;

class ExtrudePathNodeAction : public Action
{
private:
    Workspace*    m_workspace;
    Editor*       m_editor;

    PathModel*    m_model;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndices;

    unsigned int  m_startNodeIndex;
    unsigned int  m_startLineIndex;

    glm::vec3     m_startPos;
    glm::vec3     m_endPos;

    glm::vec3     m_axis;
protected:

public:
    ExtrudePathNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_pathModel, const glm::vec3& a_startPos, const glm::vec3& a_axis);
    virtual ~ExtrudePathNodeAction();

    inline void SetPosition(const glm::vec3& a_position)
    {
        m_endPos = a_position;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};