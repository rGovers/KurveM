#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "Physics/CollisionObjects/CollisionObject.h"

class Object;
class Workspace;

class SetCollisionObjectTypeAction : public Action
{
private:
    Workspace*            m_workspace;
    PhysicsEngine*        m_engine;

    unsigned int          m_objectCount;
    Object**              m_objs;

    e_CollisionObjectType m_type;

    unsigned char*        m_state;
    CollisionObject**     m_oldCObjs;

protected:

public:
    SetCollisionObjectTypeAction(e_CollisionObjectType a_type, Object* const* a_objs, unsigned int a_objectCount, Workspace* a_workspace, PhysicsEngine* a_engine);
    virtual ~SetCollisionObjectTypeAction();

    virtual e_ActionType GetActionType();

    inline void SetType(e_CollisionObjectType a_type)
    {
        m_type = a_type;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};