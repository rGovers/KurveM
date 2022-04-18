#include "PhysicsEngine.h"

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

#include "Physics/PhysicsDebugDrawer.h"

PhysicsEngine::PhysicsEngine()
{
    m_debugDraw = new PhysicsDebugDrawer();

    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);

    m_broadphaseCollision = new btDbvtBroadphase();

    m_constraintSolver = new btSequentialImpulseConstraintSolver();

    m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphaseCollision, m_constraintSolver, m_collisionConfiguration);
    m_world->setGravity(btVector3(0.0f, 9.807f, 0.0f));
    m_world->setDebugDrawer(m_debugDraw);
}
PhysicsEngine::~PhysicsEngine()
{
    delete m_world;

    delete m_constraintSolver;

    delete m_broadphaseCollision;
    
    delete m_collisionDispatcher;
    delete m_collisionConfiguration;

    delete m_debugDraw;
}

void PhysicsEngine::Update(double a_delta)
{
    m_world->stepSimulation((btScalar)a_delta);
}
void PhysicsEngine::Draw()
{
    m_world->debugDrawWorld();
}