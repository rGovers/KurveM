#pragma once

#include "Actions/Action.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class Object;

class SetPlaneCollisionShapeDirectionAction : public Action
{
private:
    unsigned int m_objectCount;

    Object**     m_objects;
    
    glm::vec3*   m_oldDir;
    glm::vec3    m_newDir;

protected:

public:
    SetPlaneCollisionShapeDirectionAction(Object* const* a_objects, unsigned int a_objectCount, const glm::vec3& a_dir);
    virtual ~SetPlaneCollisionShapeDirectionAction();

    virtual e_ActionType GetActionType();

    inline void SetDirection(const glm::vec3& a_value)
    {
        m_newDir = a_value;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};