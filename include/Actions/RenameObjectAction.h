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
    virtual ~RenameObjectAction();

    virtual e_ActionType GetActionType() const;

    void SetNewName(const char* a_newName);

    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};