#pragma once

#include "Physics/CollisionShapes/CollisionShape.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class PlaneCollisionShape : public CollisionShape
{
private:

protected:

public:
    PlaneCollisionShape(const glm::vec3& a_dir, float a_distance);
    virtual ~PlaneCollisionShape();

    virtual e_CollisionShapeType GetShapeType();

    glm::vec3 GetDirection() const;
    void SetDirection(const glm::vec3& a_dir);

    float GetDistance() const;
    void SetDistance(float a_value);
};