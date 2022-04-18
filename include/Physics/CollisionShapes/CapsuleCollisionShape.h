#pragma once

#include "Physics/CollisionShapes/CollisionShape.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class CapsuleCollisionShape : public CollisionShape
{
private:

protected:

public:
    CapsuleCollisionShape(float a_radius, float a_height);
    virtual ~CapsuleCollisionShape();

    virtual e_CollisionShapeType GetShapeType();

    float GetHeight() const;
    void SetHeight(float a_value);

    float GetRadius() const;
    void SetRadius(float a_value);
};