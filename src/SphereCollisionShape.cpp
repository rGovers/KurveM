#include "Physics/CollisionShapes/SphereCollisionShape.h"

#include <BulletCollision/CollisionShapes/btSphereShape.h>

SphereCollisionShape::SphereCollisionShape(float a_radius)
{
    m_shape = new btSphereShape(a_radius);
}
SphereCollisionShape::~SphereCollisionShape()
{
    delete m_shape;
}

e_CollisionShapeType SphereCollisionShape::GetShapeType()
{
    return CollisionShapeType_Sphere;
}

float SphereCollisionShape::GetRadius() const
{
    return (float)((btSphereShape*)m_shape)->getRadius();
}
void SphereCollisionShape::SetRadius(float a_value)
{
    ((btSphereShape*)m_shape)->setUnscaledRadius(a_value);
}