#include "Physics/CollisionShapes/PlaneCollisionShape.h"

#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>

PlaneCollisionShape::PlaneCollisionShape(const glm::vec3& a_dir, float a_distance)
{
    m_shape = new btStaticPlaneShape(btVector3(a_dir.x, a_dir.y, a_dir.z), a_distance);
}
PlaneCollisionShape::~PlaneCollisionShape()
{
    delete m_shape;
}

e_CollisionShapeType PlaneCollisionShape::GetShapeType()
{
    return CollisionShapeType_Plane;
}

glm::vec3 PlaneCollisionShape::GetDirection() const
{
    const btVector3& dir = ((btStaticPlaneShape*)m_shape)->getPlaneNormal();

    return glm::vec3(dir.x(), dir.y(), dir.z());
}
void PlaneCollisionShape::SetDirection(const glm::vec3& a_dir)
{
    const float distance = GetDistance();

    delete m_shape;

    m_shape = new btStaticPlaneShape(btVector3(a_dir.x, a_dir.y, a_dir.z), distance);
}

float PlaneCollisionShape::GetDistance() const
{
    return (float)((btStaticPlaneShape*)m_shape)->getPlaneConstant();
}
void PlaneCollisionShape::SetDistance(float a_value)
{
    const glm::vec3 dir = GetDirection();

    delete m_shape;

    m_shape = new btStaticPlaneShape(btVector3(dir.x, dir.y, dir.z), a_value);
}