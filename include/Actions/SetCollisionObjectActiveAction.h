#pragma once

#include "Actions/Action.h"

class Object;

class SetCollisionObjectActiveAction : public Action
{
private:
    unsigned int m_objectCount;

    Object**     m_objects;
    
    bool*        m_oldState;
    bool         m_newState;

protected:

public:
    SetCollisionObjectActiveAction(Object* const* a_objects, unsigned int a_objectCount, bool a_state);
    virtual ~SetCollisionObjectActiveAction();

    virtual e_ActionType GetActionType() const;

    inline void SetState(bool a_value)
    {
        m_newState = a_value;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};