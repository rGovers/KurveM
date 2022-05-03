#pragma once

#include "Physics/CollisionShapes/CollisionShape.h"

class LocalModel;
class Object;
class StridingMeshInterface;

class MeshCollisionShape : public CollisionShape
{
private:
    StridingMeshInterface* m_interface;

    LocalModel*            m_model;

    Object*                m_object;

    int                    m_steps;
    int                    m_secSteps;

protected:

public:
    MeshCollisionShape(Object* a_object);
    ~MeshCollisionShape();

    virtual e_CollisionShapeType GetShapeType();

    inline int GetSteps() const
    {
        return m_steps;
    }
    inline void SetSteps(int a_value)
    {
        m_steps = a_value;
    }
    inline int GetSecSteps() const
    {
        return m_secSteps;
    }
    inline void SetSecSteps(int a_value)
    {
        m_secSteps = a_value;
    }

    void UpdateMesh();
    void Tick();
};