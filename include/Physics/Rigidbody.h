#pragma once

#include "Physics/CollisionObject.h"

class Rigidbody : public CollisionObject
{
private:
    float m_mass;

protected:

public:
    Rigidbody(Object* a_object, PhysicsEngine* a_engine);
    virtual ~Rigidbody();

    virtual void SetActiveState(bool a_state);

    virtual e_CollisionObjectType GetCollisionObjectType();

    inline float GetMass() const
    {
        return m_mass;
    }
    void SetMass(float a_value);

    virtual void Reset();

    virtual void SetCollisionShape(CollisionShape* a_shape);
};