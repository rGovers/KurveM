#include "Actions/RenameObjectAction.h"

#include <string.h>

#include "Object.h"

RenameObjectAction::RenameObjectAction(const char* a_oldName, const char* a_newName, Object* a_object)
{
    int len = strlen(a_oldName);

    m_oldName = new char[len + 1];

    for (int i = 0; i <= len; ++i)
    {
        m_oldName[i] = a_oldName[i];
    }

    SetNewName(a_newName);

    m_object = a_object;
}
RenameObjectAction::~RenameObjectAction()
{
    delete[] m_newName;
    delete[] m_oldName;
}

void RenameObjectAction::SetNewName(const char* a_newName)
{
    const int len = (int)strlen(a_newName);

    m_newName = new char[len + 1];

    for (int i = 0; i <= len; ++i)
    {
        m_newName[i] = a_newName[i];
    }
}

e_ActionType RenameObjectAction::GetActionType() const
{
    return ActionType_RenameObject;
}

void RenameObjectAction::SetData(void* a_data)
{
    SetNewName((char*)a_data);
}

bool RenameObjectAction::Redo()
{
    return Execute();
}
bool RenameObjectAction::Execute()
{
    m_object->SetName(m_newName);

    return true;
}
bool RenameObjectAction::Revert()
{
    m_object->SetName(m_oldName);

    return true;
}