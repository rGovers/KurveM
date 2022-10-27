#include "Actions/SetRigidbodyMassAction.h"

#include "Object.h"
#include "Physics/CollisionObjects/Rigidbody.h"

SetRigidbodyMassAction::SetRigidbodyMassAction(Object* const* a_objects, unsigned int a_objectCount, float a_mass)
{
    m_objectCount = a_objectCount;

    m_newMass = a_mass;

    m_objects = new Object*[m_objectCount];
    m_oldMass = new float[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;

        if (obj->GetCollisionObjectType() == CollisionObjectType_Rigidbody)
        {
            Rigidbody* body = (Rigidbody*)obj->GetCollisionObject();

            m_oldMass[i] = body->GetMass();
        }
    }
}
SetRigidbodyMassAction::~SetRigidbodyMassAction()
{
    delete[] m_objects;
    delete[] m_oldMass;
}

e_ActionType SetRigidbodyMassAction::GetActionType() const
{
    return ActionType_SetRigidbodyMass;
}

void SetRigidbodyMassAction::SetData(void* a_data)
{
    m_newMass = *(float*)a_data;
}

bool SetRigidbodyMassAction::Redo()
{
    return Execute();
}
bool SetRigidbodyMassAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionObjectType() == CollisionObjectType_Rigidbody)
        {
            Rigidbody* body = (Rigidbody*)obj->GetCollisionObject();

            body->SetMass(m_newMass);
        }
    }

    return true;
}
bool SetRigidbodyMassAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionObjectType() == CollisionObjectType_Rigidbody)
        {
            Rigidbody* body = (Rigidbody*)obj->GetCollisionObject();

            body->SetMass(m_oldMass[i]);
        }
    }

    return true;
}