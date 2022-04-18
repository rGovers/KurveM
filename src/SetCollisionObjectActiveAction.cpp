#include "Actions/SetCollisionObjectActiveAction.h"

#include "Object.h"

SetCollisionObjectActiveAction::SetCollisionObjectActiveAction(Object* const* a_objects, unsigned int a_objectCount, bool a_state)
{
    m_newState = a_state;

    m_objectCount = a_objectCount;

    m_objects = new Object*[m_objectCount];
    m_oldState = new bool[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;
        if (obj->GetCollisionObjectType() != CollisionObjectType_Null)
        {
            m_oldState[i] = obj->GetCollisionObject()->IsActive();
        }
    }
}
SetCollisionObjectActiveAction::~SetCollisionObjectActiveAction()
{
    delete[] m_oldState;
    delete[] m_objects;
}

e_ActionType SetCollisionObjectActiveAction::GetActionType()
{
    return ActionType_SetCollisionObjectActive;
}

bool SetCollisionObjectActiveAction::Redo()
{
    return Execute();
}
bool SetCollisionObjectActiveAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        if (obj->GetCollisionObjectType() != CollisionObjectType_Null)
        {
            obj->GetCollisionObject()->SetActiveState(m_newState);
        }
    }

    return true;
}
bool SetCollisionObjectActiveAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        if (obj->GetCollisionObjectType() != CollisionObjectType_Null)
        {
            obj->GetCollisionObject()->SetActiveState(m_oldState[i]);
        }
    }

    return true;
}