#include "Physics/PhysicsDebugDrawer.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Gizmos.h"

PhysicsDebugDrawer::PhysicsDebugDrawer()
{

}
PhysicsDebugDrawer::~PhysicsDebugDrawer()
{

}

void PhysicsDebugDrawer::drawLine(const btVector3& a_from, const btVector3& a_to, const btVector3& a_color)
{
    const glm::vec3 from = glm::vec3(a_from.x(), a_from.y(), a_from.z());
    const glm::vec3 to = glm::vec3(a_to.x(), a_to.y(), a_to.z());
    const glm::vec4 color = glm::vec4(a_color.x(), a_color.y(), a_color.z(), 1.0f);

    Gizmos::DrawLine(from, to, m_cFor, 0.001f, color);
}
void PhysicsDebugDrawer::drawContactPoint(const btVector3& a_pointOnB, const btVector3& a_normalOnB, btScalar a_distance, int a_lifeTime, const btVector3& a_color)
{
    const glm::vec3 pos = glm::vec3(a_pointOnB.x(), a_pointOnB.y(), a_pointOnB.z());
    const glm::vec3 norm = glm::vec3(a_normalOnB.x(), a_normalOnB.y(), a_normalOnB.z());

    const glm::vec3 endPos = pos + norm * a_distance;

    const glm::vec4 color = glm::vec4(a_color.x(), a_color.y(), a_color.z(), 1.0f);

    Gizmos::DrawLine(pos, endPos, m_cFor, 0.025f, color);
}

void PhysicsDebugDrawer::drawTransform(const btTransform& a_transform, btScalar a_orthoLen)
{
    const btVector3 origin = a_transform.getOrigin();
    const btQuaternion q = a_transform.getRotation();

    const glm::vec3 pos = glm::vec3(origin.x(), origin.y(), origin.z());
    const glm::quat quat = glm::quat(q.x(), q.y(), q.z(), q.w());

    const glm::vec4 f = quat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    Gizmos::DrawTranslation(pos, f.xyz(), (float)a_orthoLen);
}

void PhysicsDebugDrawer::reportErrorWarning(const char* a_warningString)
{
    printf(a_warningString);
    printf("\n");
}
void PhysicsDebugDrawer::draw3dText(const btVector3& a_location, const char* a_textString)
{

}

void PhysicsDebugDrawer::setDebugMode(int a_debugMode)
{
    m_debugMode = a_debugMode;
}
int PhysicsDebugDrawer::getDebugMode() const
{
    return m_debugMode;
}