#include "Physics/TransformMotionState.h"

#include "Object.h"
#include "Transform.h"

TransformMotionState::TransformMotionState(Object* a_object)
{
    m_object = a_object;
}
TransformMotionState::~TransformMotionState()
{

}

void TransformMotionState::getWorldTransform(btTransform& a_worldTrans) const
{
    const Transform* trans = m_object->GetAnimationTransform();

    const glm::vec3& pos = trans->Translation();
    const glm::quat& quat = trans->Quaternion();

    a_worldTrans.setOrigin(btVector3(pos.x, pos.y, pos.z));
    a_worldTrans.setRotation(btQuaternion(quat.x, quat.y, quat.z, quat.w));
}
void TransformMotionState::setWorldTransform(const btTransform& a_worldTrans)
{
    const btVector3& pos = a_worldTrans.getOrigin();
    const btQuaternion quat = a_worldTrans.getRotation();

    Transform* trans = m_object->GetAnimationTransform();

    trans->Translation() = glm::vec3(pos.x(), pos.y(), pos.z());
    trans->Quaternion() = glm::quat(quat.x(), quat.y(), quat.z(), quat.w());
}