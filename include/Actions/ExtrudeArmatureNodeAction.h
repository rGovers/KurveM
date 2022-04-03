#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Object;
class Editor;

class ExtrudeArmatureNodeAction : public Action
{
private:
    Editor*       m_editor;

    unsigned int  m_nodeCount;
    Object**      m_nodes;

    glm::vec3     m_startPos;
    glm::vec3     m_endPos;

    glm::vec3     m_axis;
    
    bool          m_own;

    Object**      m_cNodes;

protected:

public:
    ExtrudeArmatureNodeAction(Editor* a_editor, Object* const* a_nodes, unsigned int a_nodeCount, const glm::vec3& a_startPos, const glm::vec3& a_axis);
    virtual ~ExtrudeArmatureNodeAction();

    void SetPosition(const glm::vec3& a_position)
    {
        m_endPos = a_position;
    }

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};