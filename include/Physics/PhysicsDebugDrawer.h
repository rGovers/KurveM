#pragma once

#include <LinearMath/btIDebugDraw.h>

class PhysicsDebugDrawer : public btIDebugDraw
{
private:
    int m_debugMode;

protected:

public:
    PhysicsDebugDrawer();
    ~PhysicsDebugDrawer();

    virtual void drawLine(const btVector3& a_from, const btVector3& a_to, const btVector3& a_color);
    virtual void drawContactPoint(const btVector3& a_pointOnB, const btVector3& a_normalOnB, btScalar a_distance, int a_lifeTime, const btVector3& a_color);

    virtual void drawTransform(const btTransform& a_transform, btScalar a_orthoLen);

    virtual void reportErrorWarning(const char* a_warningString);
    virtual void draw3dText(const btVector3& a_location, const char* a_textString);

    virtual void setDebugMode(int a_debugMode);
	virtual int getDebugMode() const;
};
