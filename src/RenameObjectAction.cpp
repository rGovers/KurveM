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

    len = strlen(a_newName);

    m_newName = new char[len + 1];

    for (int i = 0; i <= len; ++i)
    {
        m_newName[i] = a_newName[i];
    }

    m_object = a_object;
}
RenameObjectAction::~RenameObjectAction()
{
    delete[] m_newName;
    delete[] m_oldName;
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