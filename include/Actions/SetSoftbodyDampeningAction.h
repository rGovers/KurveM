#pragma once

#include "Actions/Action.h"

class Object;

class SetSoftbodyDampeningAction : public Action
{
private:
    unsigned int m_objectCount;

    Object**     m_objects;
    
    float*       m_oldDampening;
    float        m_newDampening;

protected:

public:
    SetSoftbodyDampeningAction(Object* const* a_objects, unsigned int a_objectCount, float a_dampening);
    virtual ~SetSoftbodyDampeningAction();

    virtual e_ActionType GetActionType();

    inline void SetDampening(float a_value)
    {
        m_newDampening = a_value;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};