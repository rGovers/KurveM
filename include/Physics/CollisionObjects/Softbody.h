#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Physics/CollisionObjects/CollisionObject.h"

#include <BulletSoftBody/btSoftBody.h>

class ShaderStorageBuffer;

class Softbody : public CollisionObject
{
private:
    ShaderStorageBuffer* m_deltaData;

    float                m_mass;

    btSoftBody* GenerateBody();

protected:

public:
    Softbody(Object* a_object, PhysicsEngine* a_engine);
    virtual ~Softbody();

    inline ShaderStorageBuffer* GetDeltaStorageBuffer() const
    {
        return m_deltaData;
    }

    virtual void SetActiveState(bool a_state);

    virtual e_CollisionObjectType GetCollisionObjectType();

    inline float GetMass() const
    {
        return m_mass;
    }
    void SetMass(float a_value);

    virtual void Reset();

    virtual void SetCollisionShape(CollisionShape* a_shape) { }

    void UpdateDeltaStorageBuffer();
};