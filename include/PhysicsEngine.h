#pragma once

#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/CollisionDispatch/btCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletDynamics/ConstraintSolver/btConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>

class PhysicsDebugDrawer;

class PhysicsEngine
{
private:
    PhysicsDebugDrawer*       m_debugDraw;

    btCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher*    m_collisionDispatcher;
    btBroadphaseInterface*    m_broadphase;
    btConstraintSolver*       m_constraintSolver;

    btDynamicsWorld*          m_world;
    
protected:

public:
    PhysicsEngine();
    ~PhysicsEngine();

    inline btDynamicsWorld* GetDynamicsWorld() const
    {  
        return m_world; 
    }

    void Update(double a_delta);
    void Draw();
};