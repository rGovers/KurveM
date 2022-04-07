#pragma once

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

#include "Actions/Action.h"

class PathModel;
class ShapeEditor;
class Workspace;

class ExtrudeShapeNodeAction : public Action
{
private:
    Workspace*    m_workspace;
    ShapeEditor*  m_editor;
    
    PathModel*    m_model;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndicies;

    unsigned int  m_startNodeIndex;
    unsigned int  m_startLineIndex;

    glm::vec2     m_startPos;
    glm::vec2     m_endPos;

    glm::vec2     m_axis;

protected:

public:
    ExtrudeShapeNodeAction(Workspace* a_workspace, ShapeEditor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_model, const glm::vec2& a_startPos, const glm::vec2& a_axis);
    virtual ~ExtrudeShapeNodeAction();

    inline void SetPosition(const glm::vec2& a_position)
    {
        m_endPos = a_position;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};