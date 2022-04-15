#pragma once

#include <LinearMath/btMotionState.h>

class Object;

class TransformMotionState : public btMotionState
{
private:
    Object* m_object;

protected:

public:
    TransformMotionState(Object* a_object);
    virtual ~TransformMotionState();

    virtual void getWorldTransform(btTransform& a_worldTrans) const;
    virtual void setWorldTransform(const btTransform& a_worldTrans);
};