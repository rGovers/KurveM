#include "Actions/DeleteObjectAction.h"

#include "Object.h"
#include "Workspace.h"

DeleteObjectAction::DeleteObjectAction(Workspace* a_workspace, Object* a_object)
{
    m_workspace = a_workspace;

    m_object = a_object;
    m_parentObject = m_object->GetParent();
}
DeleteObjectAction::~DeleteObjectAction()
{
    if (m_held)
    {
        delete m_object;
    }
}

bool DeleteObjectAction::Redo()
{
    return Execute();
}
bool DeleteObjectAction::Execute()
{
    if (m_parentObject != nullptr)
    {
        m_object->SetParent(nullptr);
    }
    else
    {
        m_workspace->RemoveObject(m_object);
    }

    m_held = true;

    return true;
}
bool DeleteObjectAction::Revert()
{
    if (m_parentObject != nullptr)
    {
        m_object->SetParent(m_parentObject);
    }
    else
    {
        m_workspace->AddObject(m_object);
    }

    m_held = false;

    return true;
}