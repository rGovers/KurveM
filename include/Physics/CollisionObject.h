#pragma once

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

class CollisionShape;
class Object;
class PhysicsEngine;
class TransformMotionState;

enum e_CollisionObjectType
{
    CollisionObjectType_Null,
    CollisionObjectType_CollisionObject,
    CollisionObjectType_Rigidbody,
    CollisionObjectType_End
};

class CollisionObject
{
private:

protected:
    bool                  m_isActive;

    PhysicsEngine*        m_engine;

    TransformMotionState* m_transformState;

    Object*               m_object;
    CollisionShape*       m_shape;
    
    btCollisionObject*    m_collisionObject;

    CollisionObject();

public:
    CollisionObject(Object* a_object, PhysicsEngine* a_engine);
    virtual ~CollisionObject();

    inline bool IsActive() const
    {
        return m_isActive;
    }
    virtual void SetActiveState(bool a_state);

    virtual e_CollisionObjectType GetCollisionObjectType();

    virtual void SetCollisionShape(CollisionShape* a_shape);
    inline CollisionShape* GetCollisionShape() const
    {
        return m_shape;
    }
};