#pragma once

#include "Actions/Action.h"

class Object;
class Workspace;

class DeleteObjectAction : public Action
{
private:
    Workspace* m_workspace;

    Object*    m_parentObject;
    Object*    m_object;

    bool       m_held;
    bool       m_state;

protected:

public:
    DeleteObjectAction(Workspace* a_workspace, Object* a_object);
    virtual ~DeleteObjectAction();

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};