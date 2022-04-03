#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class CurveModel;
class Workspace;

class RotateCurveNodeAction : public Action
{
private:
    Workspace*    m_workspace;

    CurveModel*   m_curveModel;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndices;
    
    glm::vec3*    m_oldPos;
    glm::vec3     m_startPos;
    glm::vec3     m_endPos;

    glm::vec3     m_centre;

    glm::vec3     m_axis;
protected:

public:
    RotateCurveNodeAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis);
    virtual ~RotateCurveNodeAction();

    inline void SetRotation(const glm::vec3& a_pos)
    {
        m_endPos = a_pos;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};