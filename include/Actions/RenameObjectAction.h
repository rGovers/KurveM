#pragma once

#include "Actions/Action.h"

class Object;

class RenameObjectAction : public Action
{
private:
    char*   m_oldName;
    char*   m_newName;

    Object* m_object;

protected:

public:
    RenameObjectAction(const char* a_oldName, const char* a_newName, Object* a_object);
    ~RenameObjectAction();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};