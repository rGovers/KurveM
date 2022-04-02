#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class PathModel;
class Workspace;

class MoveShapeNodeAction : public Action
{
private:
    Workspace*    m_workspace;

    PathModel*    m_model;

    unsigned int* m_indices; 
    unsigned int  m_indexCount;

    glm::vec2*    m_oldPos;
    glm::vec2     m_startPos;
    glm::vec2     m_endPos;

    glm::vec2     m_axis;

protected:

public:
    MoveShapeNodeAction(Workspace* a_workspace, const unsigned int* a_indices, unsigned int a_indexCount, PathModel* a_pathModel, const glm::vec2& a_pos, const glm::vec2& a_axis);
    virtual ~MoveShapeNodeAction();

    void SetPosition(const glm::vec2& a_pos)
    {
        m_endPos = a_pos;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};