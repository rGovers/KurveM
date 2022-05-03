#include "Actions/SetSoftbodyLineVolumeStiffnessAction.h"

#include "Object.h"
#include "Physics/CollisionObjects/Softbody.h"

SetSoftbodyLineVolumeStiffnessAction::SetSoftbodyLineVolumeStiffnessAction(Object* const* a_objects, unsigned int a_objectCount, float a_stiffness)
{
    m_objectCount = a_objectCount;

    m_newStiffness = a_stiffness;

    m_objects = new Object*[m_objectCount];
    m_oldStiffness = new float[m_objectCount];
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;

        if (obj->GetCollisionObjectType() == CollisionObjectType_Softbody)
        {
            const Softbody* body = (Softbody*)obj->GetCollisionObject();

            m_oldStiffness[i] = body->GetLineVolumeStiffness();
        }
    }
}
SetSoftbodyLineVolumeStiffnessAction::~SetSoftbodyLineVolumeStiffnessAction()
{
    delete[] m_objects;
    delete[] m_oldStiffness;
}

e_ActionType SetSoftbodyLineVolumeStiffnessAction::GetActionType()
{
    return ActionType_SetSoftbodyLineVolumeStiffness;
}

void SetSoftbodyLineVolumeStiffnessAction::SetData(void* a_data)
{
    m_newStiffness = *(float*)a_data;
}

bool SetSoftbodyLineVolumeStiffnessAction::Redo()
{
    return Execute();
}
bool SetSoftbodyLineVolumeStiffnessAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionObjectType() == CollisionObjectType_Softbody)
        {
            Softbody* body = (Softbody*)obj->GetCollisionObject();

            body->SetLineVolumeStiffness(m_newStiffness);
        }
    }

    return true;
}
bool SetSoftbodyLineVolumeStiffnessAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionObjectType() == CollisionObjectType_Softbody)
        {
            Softbody* body = (Softbody*)obj->GetCollisionObject();

            body->SetLineVolumeStiffness(m_oldStiffness[i]);
        }
    }

    return true;
}