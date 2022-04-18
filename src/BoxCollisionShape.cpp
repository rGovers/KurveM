#include "Physics/CollisionShapes/BoxCollisionShape.h"

#include <BulletCollision/CollisionShapes/btBoxShape.h>

BoxCollisionShape::BoxCollisionShape(const glm::vec3& a_halfExtents)
{
    m_shape = new btBoxShape(btVector3(a_halfExtents.x, a_halfExtents.y, a_halfExtents.z));
}
BoxCollisionShape::~BoxCollisionShape()
{
    delete m_shape;
}

e_CollisionShapeType BoxCollisionShape::GetShapeType()
{
    return CollisionShapeType_Box;
}

glm::vec3 BoxCollisionShape::GetHalfExtents() const
{
    const btVector3& vec = ((btBoxShape*)m_shape)->getHalfExtentsWithMargin();

    return glm::vec3(vec.x(), vec.y(), vec.z());
}
void BoxCollisionShape::SetHalfExtents(const glm::vec3& a_value)
{
    btBoxShape* box = (btBoxShape*)m_shape;

    const btVector3& lS = box->getLocalScaling();

    const glm::vec3 val = (a_value * glm::vec3(lS.x(), lS.y(), lS.z())) - glm::vec3((float)box->getMargin());

    box->setImplicitShapeDimensions(btVector3(val.x, val.y, val.z));
}