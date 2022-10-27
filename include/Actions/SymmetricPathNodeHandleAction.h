#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "EditorControls/Editor.h"

class PathModel;
class Workspace;

class SymmetricPathNodeHandleAction : public Action
{
private:
    Workspace*     m_workspace;
 
    PathModel*     m_pathModel;
 
    unsigned int   m_nodeCount;
    unsigned int*  m_nodeIndices;
    unsigned int** m_mirrorIndices;

    glm::vec3**    m_handles;

    glm::vec3 GetMirrorMultiplier(e_MirrorMode a_mode) const;

protected:

public:
    SymmetricPathNodeHandleAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_model, e_MirrorMode a_mirrorMode);
    virtual ~SymmetricPathNodeHandleAction();

    virtual e_ActionType GetActionType() const;

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};