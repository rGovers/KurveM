#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "Physics/CollisionObjects/CollisionObject.h"

class Object;

class SetCollisionObjectTypeAction : public Action
{
private:
    PhysicsEngine*        m_engine;

    unsigned int          m_objectCount;
    Object**              m_objs;

    e_CollisionObjectType m_type;

    bool                  m_own;
    bool*                 m_state;
    CollisionObject**     m_oldCObjs;

protected:

public:
    SetCollisionObjectTypeAction(e_CollisionObjectType a_type, Object* const* a_objs, unsigned int a_objectCount, PhysicsEngine* a_engine);
    virtual ~SetCollisionObjectTypeAction();

    virtual e_ActionType GetActionType();

    inline void SetType(e_CollisionObjectType a_type)
    {
        m_type = a_type;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};