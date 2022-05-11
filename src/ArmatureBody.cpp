#include "Physics/ArmatureBody.h"

#include <BulletCollision/CollisionShapes/btSphereShape.h>

#include "Physics/TransformMotionState.h"
#include "PhysicsEngine.h"

ArmatureBody::ArmatureBody(Object* a_object, PhysicsEngine* a_engine)
{
    m_engine = a_engine;

    m_motionState = new TransformMotionState(a_object);
    m_shape = new btSphereShape(0.1f);

    m_body = new btRigidBody(1.0f, m_motionState, m_shape);
    m_body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);

    m_engine->GetDynamicsWorld()->addRigidBody(m_body);
}
ArmatureBody::~ArmatureBody()
{
    m_engine->GetDynamicsWorld()->removeRigidBody(m_body);

    delete m_body;
    delete m_shape;
    delete m_motionState;
}
