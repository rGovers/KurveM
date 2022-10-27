#pragma once

#include "Actions/Action.h"

class Object;

class SetCapsuleCollisionShapeRadiusAction : public Action
{
private:
    unsigned int m_objectCount;

    Object**     m_objects;
    
    float*       m_oldRadius;
    float        m_newRadius;

protected:

public:
    SetCapsuleCollisionShapeRadiusAction(Object* const* a_objects, unsigned int a_objectCount, float a_radius);
    virtual ~SetCapsuleCollisionShapeRadiusAction();

    virtual e_ActionType GetActionType() const;

    inline void SetRadius(float a_value)
    {
        m_newRadius = a_value;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};