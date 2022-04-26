#pragma once

#include "Actions/Action.h"

class Object;
class Workspace;

class SetPathArmatureAction : public Action
{
private:
    Workspace*   m_workspace;

    long long*   m_oldID;
    long long    m_newID; 

    unsigned int m_objectCount;

    Object**     m_objects;

protected:

public:
    SetPathArmatureAction(Workspace* a_workspace, Object* const* a_objects, unsigned int a_objectCount, long long a_id);
    virtual ~SetPathArmatureAction();

    virtual e_ActionType GetActionType();

    inline void SetID(long long a_value)
    {
        m_newID = a_value;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};