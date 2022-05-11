#pragma once

#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

class Object;
class PhysicsEngine;
class TransformMotionState;

class ArmatureBody
{
private:
    PhysicsEngine*        m_engine;

    TransformMotionState* m_motionState;
    btCollisionShape*     m_shape;
    btRigidBody*          m_body;

protected:

public:
    ArmatureBody(Object* a_object, PhysicsEngine* a_engine);
    ~ArmatureBody();

    inline btRigidBody* GetRigidbody() const
    {
        return m_body;
    }
};