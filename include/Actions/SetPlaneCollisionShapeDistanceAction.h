#pragma once

#include "Actions/Action.h"

class Object;

class SetPlaneCollisionShapeDistanceAction : public Action
{
private:
    unsigned int m_objectCount;

    Object**     m_objects;
    
    float*       m_oldDist;
    float        m_newDist;

protected:

public:
    SetPlaneCollisionShapeDistanceAction(Object* const* a_objects, unsigned int a_objectCount, float a_distance);
    virtual ~SetPlaneCollisionShapeDistanceAction();

    virtual e_ActionType GetActionType();

    inline void SetDistance(float a_value)
    {
        m_newDist = a_value;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};