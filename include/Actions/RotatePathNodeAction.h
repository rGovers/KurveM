#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "EditorControls/Editor.h"

class PathModel;
class Workspace;

class RotatePathNodeAction : public Action
{
private:
    Workspace*     m_workspace;
 
    PathModel*     m_pathModel;
 
    unsigned int   m_nodeCount;
    unsigned int*  m_nodeIndices;

    unsigned int** m_mirrorIndices;

    float*         m_startRotation;
    glm::vec3      m_startPos;
    glm::vec3      m_endPos;
 
    glm::vec3      m_axis;

protected:

public:
    RotatePathNodeAction(Workspace* a_workspace, const unsigned int* a_indices, unsigned int a_indexCount, PathModel* a_model, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode);
    virtual ~RotatePathNodeAction();

    virtual e_ActionType GetActionType() const;

    inline void SetRotation(const glm::vec3& a_pos)
    {
        m_endPos = a_pos;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};