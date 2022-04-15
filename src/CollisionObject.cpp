#include "Physics/CollisionObject.h"

#include "Object.h"
#include "Physics/CollisionShapes/CollisionShape.h"
#include "Physics/TransformMotionState.h"
#include "PhysicsEngine.h"

CollisionObject::CollisionObject()
{
    m_isActive = true;

    m_engine = nullptr;
    m_object = nullptr;
    m_collisionObject = nullptr;
    m_shape = nullptr;
    m_transformState = nullptr;
}
CollisionObject::CollisionObject(Object* a_object, PhysicsEngine* a_engine) : CollisionObject()
{
    m_engine = a_engine;

    m_object = a_object;

    m_transformState = new TransformMotionState(m_object);
}
CollisionObject::~CollisionObject()
{
    if (m_collisionObject != nullptr)
    {
        m_engine->GetDynamicsWorld()->removeCollisionObject(m_collisionObject);

        delete m_collisionObject;
        m_collisionObject = nullptr;
    }
    
    if (m_transformState != nullptr)
    {
        delete m_transformState;
        m_transformState = nullptr;
    }
}

void CollisionObject::SetActiveState(bool a_state)
{
    m_isActive = a_state;
    
    if (m_isActive && m_collisionObject == nullptr && m_shape != nullptr)
    {
        m_collisionObject = new btCollisionObject();

        m_collisionObject->setCollisionShape(m_shape->GetShape());

        btTransform trans;
        m_transformState->getWorldTransform(trans);
        m_collisionObject->setWorldTransform(trans);
        
        m_engine->GetDynamicsWorld()->addCollisionObject(m_collisionObject); 
    }
    else if (m_collisionObject != nullptr)
    {
        m_engine->GetDynamicsWorld()->removeCollisionObject(m_collisionObject);

        delete m_collisionObject;
        m_collisionObject = nullptr;
    }
}

e_CollisionObjectType CollisionObject::GetCollisionObjectType()
{
    return CollisionObjectType_CollisionObject;
}

void CollisionObject::SetCollisionShape(CollisionShape* a_shape)
{
    m_shape = a_shape;

    if (m_isActive && m_shape != nullptr)
    {
        if (m_collisionObject == nullptr)
        {
            m_collisionObject = new btCollisionObject();

            m_collisionObject->setCollisionShape(m_shape->GetShape());

            btTransform trans;
            m_transformState->getWorldTransform(trans);
            m_collisionObject->setWorldTransform(trans);

            m_engine->GetDynamicsWorld()->addCollisionObject(m_collisionObject); 
        }
        else
        {
            m_collisionObject->setCollisionShape(m_shape->GetShape());
        }
    }
    else if (m_collisionObject != nullptr)
    {
        m_engine->GetDynamicsWorld()->removeCollisionObject(m_collisionObject);

        delete m_collisionObject;
        m_collisionObject = nullptr;
    }
}