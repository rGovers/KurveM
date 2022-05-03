#include "Actions/DeleteObjectAction.h"

#include "Object.h"
#include "Physics/CollisionObjects/CollisionObject.h"
#include "Workspace.h"

DeleteObjectAction::DeleteObjectAction(Workspace* a_workspace, Object* a_object)
{
    m_workspace = a_workspace;

    m_object = a_object;
    m_parentObject = m_object->GetParent();

    const CollisionObject* cObj = m_object->GetCollisionObject();
    if (cObj != nullptr)
    {
        m_state = cObj->IsActive();
    }
}
DeleteObjectAction::~DeleteObjectAction()
{
    if (m_held)
    {
        delete m_object;
    }
}

e_ActionType DeleteObjectAction::GetActionType() 
{
    return ActionType_DeleteObject;
}

bool DeleteObjectAction::Redo()
{
    return Execute();
}
bool DeleteObjectAction::Execute()
{
    CollisionObject* cObj = m_object->GetCollisionObject();
    if (cObj != nullptr)
    {
        cObj->SetActiveState(false);
    }

    if (m_parentObject != nullptr)
    {
        m_object->SetParent(nullptr);
    }
    else
    {
        m_workspace->RemoveObject(m_object);
    }

    m_workspace->RemoveSelectedObject(m_object);

    m_held = true;

    return true;
}
bool DeleteObjectAction::Revert()
{
    CollisionObject* cObj = m_object->GetCollisionObject();
    if (cObj != nullptr)
    {
        cObj->SetActiveState(m_state);
    }

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