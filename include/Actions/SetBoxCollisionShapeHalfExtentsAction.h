#pragma once

#include "Actions/Action.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class Object;

class SetBoxCollisionShapeHalfExtentsAction : public Action
{
private:
    unsigned int m_objectCount;

    Object**     m_objects;
    
    glm::vec3*   m_oldHalfExtents;
    glm::vec3    m_newHalfExtents;

protected:

public:
    SetBoxCollisionShapeHalfExtentsAction(Object* const* a_objects, unsigned int a_objectCount, const glm::vec3& a_halfExtents);
    virtual ~SetBoxCollisionShapeHalfExtentsAction();

    virtual e_ActionType GetActionType();

    inline void SetHalfExtents(const glm::vec3& a_value)
    {
        m_newHalfExtents = a_value;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};