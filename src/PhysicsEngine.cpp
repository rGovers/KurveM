#include "PhysicsEngine.h"

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "Camera.h"
#include "Physics/PhysicsDebugDrawer.h"
#include "Transform.h"

PhysicsEngine::PhysicsEngine()
{
    const btVector3 gravity = btVector3(0.0f, 9.807f, 0.0f);

    m_debugDraw = new PhysicsDebugDrawer();

    m_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
    m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);

    m_broadphase = new btDbvtBroadphase();

    m_constraintSolver = new btSequentialImpulseConstraintSolver();

    btSoftRigidDynamicsWorld* world = new btSoftRigidDynamicsWorld(m_collisionDispatcher, m_broadphase, m_constraintSolver, m_collisionConfiguration);

    world->setGravity(gravity);
    world->getWorldInfo().m_gravity = gravity;
    world->setDebugDrawer(m_debugDraw);

    m_world = world;
}
PhysicsEngine::~PhysicsEngine()
{
    delete m_world;

    delete m_constraintSolver;

    delete m_broadphase;
    
    delete m_collisionDispatcher;
    delete m_collisionConfiguration;

    delete m_debugDraw;
}

void PhysicsEngine::Update(double a_delta)
{
    m_world->stepSimulation((btScalar)a_delta);
}
void PhysicsEngine::Draw(const Camera* a_camera)
{
    m_debugDraw->SetForward(a_camera->GetTransform()->Forward());

    m_world->debugDrawWorld();

    btCollisionObjectArray arr = m_world->getCollisionObjectArray();
    const int size = arr.size();
    for (int i = 0; i < size; ++i)
    {
        const btCollisionObject* cObj = arr[i];

        const btTransform& transform = cObj->getWorldTransform();
        const btCollisionShape* shape = cObj->getCollisionShape();
        m_world->debugDrawObject(transform, shape, btVector3(0.0f, 1.0f, 0.0f));
    }
}