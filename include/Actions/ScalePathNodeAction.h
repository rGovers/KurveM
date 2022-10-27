#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "EditorControls/Editor.h"

class PathModel;
class Workspace;

class ScalePathNodeAction : public Action
{
private:
    Workspace*     m_workspace;
    PathModel*     m_model;
 
    unsigned int   m_indexCount;
    unsigned int*  m_indices;
    unsigned int** m_mirrorIndices;

    glm::vec2*     m_startScale;
    glm::vec3      m_startPos;
    glm::vec3      m_endPos;
 
    glm::vec3      m_axis;

protected:

public:
    ScalePathNodeAction(Workspace* a_workspace, const unsigned int* a_indices, unsigned int a_indexCount, PathModel* a_model, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode);
    virtual ~ScalePathNodeAction();

    virtual e_ActionType GetActionType() const;

    inline void SetScale(const glm::vec3& a_endPos)
    {
        m_endPos = a_endPos;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};