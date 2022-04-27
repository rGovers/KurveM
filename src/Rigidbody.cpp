#include "Physics/CollisionObjects/Rigidbody.h"

#include <BulletDynamics/Dynamics/btRigidBody.h>

#include "Physics/CollisionShapes/CollisionShape.h"
#include "Physics/TransformMotionState.h"
#include "PhysicsEngine.h"

Rigidbody::Rigidbody(Object* a_object, PhysicsEngine* a_engine) : CollisionObject()
{
    m_engine = a_engine;

    m_object = a_object;

    m_transformState = new TransformMotionState(m_object);

    m_mass = 1.0f;
}
Rigidbody::~Rigidbody()
{
    if (m_collisionObject != nullptr)
    {
        m_engine->GetDynamicsWorld()->removeRigidBody((btRigidBody*)m_collisionObject);

        delete m_collisionObject;
        m_collisionObject = nullptr;
    }

    if (m_transformState != nullptr)
    {
        delete m_transformState;
        m_transformState = nullptr;
    }
}

void Rigidbody::SetActiveState(bool a_state)
{
    m_isActive = a_state;

    if (m_isActive && m_collisionObject == nullptr && m_shape != nullptr)
    {
        btRigidBody* body = new btRigidBody(m_mass, m_transformState, m_shape->GetShape());

        m_collisionObject = body;
        m_engine->GetDynamicsWorld()->addRigidBody(body);
    }
    else if (m_collisionObject != nullptr)
    {
        m_engine->GetDynamicsWorld()->removeRigidBody((btRigidBody*)m_collisionObject);
            
        delete m_collisionObject;
        m_collisionObject = nullptr;
    }
}

e_CollisionObjectType Rigidbody::GetCollisionObjectType()
{
    return CollisionObjectType_Rigidbody;
}

void Rigidbody::SetMass(float a_value)
{
    m_mass = a_value;

    if (m_collisionObject != nullptr)
    {
        ((btRigidBody*)m_collisionObject)->setMassProps(m_mass, btVector3(0.0f, 0.0f, 0.0f));
    }
}

void Rigidbody::Reset()
{
    if (m_collisionObject != nullptr)
    {
        btRigidBody* body = (btRigidBody*)m_collisionObject;

        // Need to do this because bullet does not auto update
        btTransform transform;
        m_transformState->getWorldTransform(transform);
        body->setCenterOfMassTransform(transform);

        body->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));

        body->clearForces();
    }
}

void Rigidbody::SetCollisionShape(CollisionShape* a_shape)
{
    m_shape = a_shape;
    
    if (m_isActive && m_shape != nullptr)
    {
        if (m_collisionObject == nullptr)
        {
            btRigidBody* body = new btRigidBody(m_mass, m_transformState, m_shape->GetShape());

            m_collisionObject = body;
            m_engine->GetDynamicsWorld()->addRigidBody(body);
        }
        else
        {
            m_collisionObject->setCollisionShape(m_shape->GetShape());
        }
    }
    else if (m_collisionObject != nullptr)
    {
        m_engine->GetDynamicsWorld()->removeRigidBody((btRigidBody*)m_collisionObject);
            
        delete m_collisionObject;
        m_collisionObject = nullptr;
    }
}