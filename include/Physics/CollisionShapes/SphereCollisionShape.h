#pragma once

#include "Physics/CollisionShapes/CollisionShape.h"

class SphereCollisionShape : public CollisionShape
{
private:

protected:

public:
    SphereCollisionShape(float a_radius);
    virtual ~SphereCollisionShape();

    virtual e_CollisionShapeType GetShapeType();

    float GetRadius() const;
    void SetRadius(float a_value);
};