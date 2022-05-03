#pragma once

#include "Actions/Action.h"

class Object;

class SetSoftbodyMassAction : public Action
{
private:
    unsigned int m_objectCount;

    Object**     m_objects;
    
    float*       m_oldMass;
    float        m_newMass;

protected:

public:
    SetSoftbodyMassAction(Object* const* a_objects, unsigned int a_objectCount, float a_mass);
    virtual ~SetSoftbodyMassAction();

    virtual e_ActionType GetActionType();

    inline void SetMass(float a_value)
    {
        m_newMass = a_value;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};