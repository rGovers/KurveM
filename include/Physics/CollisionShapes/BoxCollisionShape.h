#pragma once

#include "Physics/CollisionShapes/CollisionShape.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class BoxCollisionShape : public CollisionShape
{
private:

protected:

public:
    BoxCollisionShape(const glm::vec3& a_halfExtents);
    virtual ~BoxCollisionShape();

    virtual e_CollisionShapeType GetShapeType();

    glm::vec3 GetHalfExtents() const;
    void SetHalfExtents(const glm::vec3& a_value);
};