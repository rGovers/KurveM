#include "Actions/SetSoftbodyMassAction.h"

#include "Object.h"
#include "Physics/CollisionObjects/Softbody.h"

SetSoftbodyMassAction::SetSoftbodyMassAction(Object* const* a_objects, unsigned int a_objectCount, float a_mass)
{
    m_objectCount = a_objectCount;

    m_newMass = a_mass;

    m_objects = new Object*[m_objectCount];
    m_oldMass = new float[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;

        if (obj->GetCollisionObjectType() == CollisionObjectType_Softbody)
        {
            const Softbody* body = (Softbody*)obj->GetCollisionObject();

            m_oldMass[i] = body->GetMass();
        }
    }
}
SetSoftbodyMassAction::~SetSoftbodyMassAction()
{
    delete[] m_objects;
    delete[] m_oldMass;
}

e_ActionType SetSoftbodyMassAction::GetActionType() const
{
    return ActionType_SetSoftbodyMass;
}

void SetSoftbodyMassAction::SetData(void* a_data)
{
    m_newMass = *(float*)a_data;
}

bool SetSoftbodyMassAction::Redo()
{
    return Execute();
}
bool SetSoftbodyMassAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionObjectType() == CollisionObjectType_Softbody)
        {
            Softbody* body = (Softbody*)obj->GetCollisionObject();

            body->SetMass(m_newMass);
        }
    }

    return true;
}
bool SetSoftbodyMassAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionObjectType() == CollisionObjectType_Softbody)
        {
            Softbody* body = (Softbody*)obj->GetCollisionObject();

            body->SetMass(m_oldMass[i]);
        }
    }

    return true;
}