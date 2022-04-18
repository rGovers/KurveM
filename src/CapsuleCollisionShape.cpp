#include "Physics/CollisionShapes/CapsuleCollisionShape.h"

#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

CapsuleCollisionShape::CapsuleCollisionShape(float a_radius, float a_height)
{
    m_shape = new btCapsuleShape(a_radius, a_height);
}
CapsuleCollisionShape::~CapsuleCollisionShape()
{
    delete m_shape;
}

e_CollisionShapeType CapsuleCollisionShape::GetShapeType()
{
    return CollisionShapeType_Capsule;
}

float CapsuleCollisionShape::GetHeight() const
{
    return (float)((btCapsuleShape*)m_shape)->getHalfHeight() * 2;
}
void CapsuleCollisionShape::SetHeight(float a_value)
{
    btCapsuleShape* shape = (btCapsuleShape*)m_shape;

    const btScalar radius = shape->getRadius();
    shape->setImplicitShapeDimensions(btVector3(radius, a_value * 0.5f, radius));
}

float CapsuleCollisionShape::GetRadius() const
{
    return (float)((btCapsuleShape*)m_shape)->getRadius();
}
void CapsuleCollisionShape::SetRadius(float a_value)
{
    btCapsuleShape* shape = (btCapsuleShape*)m_shape;

    const btScalar height = shape->getHalfHeight();
    shape->setImplicitShapeDimensions(btVector3(a_value, height, a_value));
}