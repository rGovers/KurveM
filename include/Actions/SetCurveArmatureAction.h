#pragma once

#include "Actions/Action.h"

class Object;
class Workspace;

class SetCurveArmatureAction : public Action
{
private:
    Workspace*   m_workspace;

    long long*   m_oldID;
    long long    m_newID; 

    unsigned int m_objectCount;

    Object**     m_objects;

protected:

public:
    SetCurveArmatureAction(Workspace* a_workspace, Object** a_objects, unsigned int a_objectCount, long long a_id);
    ~SetCurveArmatureAction();

    virtual e_ActionType GetActionType();

    inline void SetID(long long a_id)
    {
        m_newID = a_id;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};