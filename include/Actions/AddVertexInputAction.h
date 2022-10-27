#pragma once

#include "Actions/Action.h"

class Object;

class AddVertexInputAction : public Action
{
private:
    Object**     m_objs;
    unsigned int m_objectCount;

    bool         m_pushed;

protected:

public:
    AddVertexInputAction(Object* const* a_objs, unsigned int a_objectCount);
    virtual ~AddVertexInputAction();

    virtual e_ActionType GetActionType() const; 

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};