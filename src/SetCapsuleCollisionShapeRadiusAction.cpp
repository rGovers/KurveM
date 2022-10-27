#include "Actions/SetCapsuleCollisionShapeRadiusAction.h"

#include "Object.h"
#include "Physics/CollisionShapes/CapsuleCollisionShape.h"

SetCapsuleCollisionShapeRadiusAction::SetCapsuleCollisionShapeRadiusAction(Object* const* a_objects, unsigned int a_objectCount, float a_radius)
{
    m_objectCount = a_objectCount;

    m_newRadius = a_radius;

    m_objects = new Object*[m_objectCount];
    m_oldRadius = new float[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;
        if (obj->GetCollisionShapeType() == CollisionShapeType_Capsule)
        {
            m_oldRadius[i] = ((CapsuleCollisionShape*)obj->GetCollisionShape())->GetRadius();
        }
    }
}
SetCapsuleCollisionShapeRadiusAction::~SetCapsuleCollisionShapeRadiusAction()
{
    delete[] m_oldRadius;
    delete[] m_objects;
}

e_ActionType SetCapsuleCollisionShapeRadiusAction::GetActionType() const
{
    return ActionType_SetCapsuleCollisionShapeRadius;
}

void SetCapsuleCollisionShapeRadiusAction::SetData(void* a_data)
{
    m_newRadius = *(float*)a_data;
}

bool SetCapsuleCollisionShapeRadiusAction::Redo()
{
    return Execute();
}
bool SetCapsuleCollisionShapeRadiusAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i]; 
        if (obj->GetCollisionShapeType() == CollisionShapeType_Capsule)
        {
            ((CapsuleCollisionShape*)obj->GetCollisionShape())->SetRadius(m_newRadius);
        }
    }

    return true;
}
bool SetCapsuleCollisionShapeRadiusAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i]; 
        if (obj->GetCollisionShapeType() == CollisionShapeType_Capsule)
        {
            ((CapsuleCollisionShape*)obj->GetCollisionShape())->SetRadius(m_oldRadius[i]);
        }
    }

    return true;
}