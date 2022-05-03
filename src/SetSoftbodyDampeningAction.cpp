#include "Actions/SetSoftbodyDampeningAction.h"

#include "Object.h"
#include "Physics/CollisionObjects/Softbody.h"

SetSoftbodyDampeningAction::SetSoftbodyDampeningAction(Object* const* a_objects, unsigned int a_objectCount, float a_dampening)
{
    m_objectCount = a_objectCount;

    m_newDampening = a_dampening;
    
    m_oldDampening = new float[m_objectCount];
    m_objects = new Object*[m_objectCount];
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;

        if (obj->GetCollisionObjectType() == CollisionObjectType_Softbody)
        {
            const Softbody* body = (Softbody*)obj->GetCollisionObject();

            m_oldDampening[i] = body->GetDampening();
        }
    }
}
SetSoftbodyDampeningAction::~SetSoftbodyDampeningAction()
{
    delete[] m_oldDampening;
    delete[] m_objects;
}

e_ActionType SetSoftbodyDampeningAction::GetActionType()
{
    return ActionType_SetSoftbodyDampening;
}

void SetSoftbodyDampeningAction::SetData(void* a_data)
{
    m_newDampening = *(float*)a_data;
}

bool SetSoftbodyDampeningAction::Redo()
{
    return Execute();
}
bool SetSoftbodyDampeningAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionObjectType() == CollisionObjectType_Softbody)
        {
            Softbody* body = (Softbody*)obj->GetCollisionObject();

            body->SetDampening(m_newDampening);
        }
    }

    return true;
}
bool SetSoftbodyDampeningAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionObjectType() == CollisionObjectType_Softbody)
        {
            Softbody* body = (Softbody*)obj->GetCollisionObject();

            body->SetDampening(m_oldDampening[i]);
        }
    }
    
    return true;
}