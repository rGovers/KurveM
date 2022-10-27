#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "Physics/CollisionShapes/CollisionShape.h"

class Object;

class SetCollisionShapeTypeAction : public Action
{
private:
    unsigned int         m_objectCount;
    Object**             m_objs;

    e_CollisionShapeType m_type;

    bool                 m_own;
    CollisionShape**     m_oldShapes;

protected:

public:
    SetCollisionShapeTypeAction(e_CollisionShapeType a_type, Object* const* a_objs, unsigned int a_objectCount);
    virtual ~SetCollisionShapeTypeAction();

    virtual e_ActionType GetActionType() const;

    inline void SetType(e_CollisionShapeType a_type)
    {
        m_type = a_type;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};