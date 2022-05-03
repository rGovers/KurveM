#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Physics/CollisionObjects/CollisionObject.h"

#include <BulletSoftBody/btSoftBody.h>

class ShaderStorageBuffer;

class Softbody : public CollisionObject
{
private:
    ShaderStorageBuffer*  m_deltaData;

    float                 m_mass;

    float                 m_dampening;

    float                 m_stiffness;
    float                 m_angularStiffness;
    float                 m_volumeStiffness;

    float                 m_faceStiffness;
    float                 m_faceAngularStiffness;
    float                 m_faceVolumeStiffness;

    btSoftBody::Material* m_lineMat;
    btSoftBody::Material* m_faceMat;

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

    inline float GetDampening() const
    {
        return m_dampening;
    }
    void SetDampening(float a_value);

    inline float GetLineStiffness() const
    {
        return m_stiffness;
    }
    void SetLineStiffness(float a_value);
    inline float GetLineAngularStiffness() const
    {
        return m_angularStiffness;
    }
    void SetLineAngularStiffness(float a_value);
    inline float GetLineVolumeStiffness() const
    {
        return m_volumeStiffness;
    }
    void SetLineVolumeStiffness(float a_value);

    inline float GetFaceStiffness() const
    {
        return m_faceStiffness;
    }
    void SetFaceStiffness(float a_value);
    inline float GetFaceAngularStiffness() const
    {
        return m_faceAngularStiffness;
    }
    void SetFaceAngularStiffness(float a_value);
    inline float GetFaceVolumeStiffness() const
    {
        return m_faceVolumeStiffness;
    }
    void SetFaceVolumeStiffness(float a_value);

    virtual void Reset();

    virtual void SetCollisionShape(CollisionShape* a_shape) { }

    void UpdateDeltaStorageBuffer();
};