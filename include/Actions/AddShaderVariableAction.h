#pragma once

#include "Actions/Action.h"

class Object;

class AddShaderVariableAction : public Action
{
private:
    Object**     m_objs;
    unsigned int m_objectCount;

    bool         m_pushed;

protected:

public:
    AddShaderVariableAction(Object* const* a_objs, unsigned int a_objectCount);
    virtual ~AddShaderVariableAction();

    virtual e_ActionType GetActionType() const; 

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};