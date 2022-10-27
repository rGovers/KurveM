#include "Actions/SetCapsuleCollisionShapeHeightAction.h"

#include "Object.h"
#include "Physics/CollisionShapes/CapsuleCollisionShape.h"

SetCapsuleCollisionShapeHeightAction::SetCapsuleCollisionShapeHeightAction(Object* const* a_objects, unsigned int a_objectCount, float a_height)
{
    m_objectCount = a_objectCount;

    m_newHeight = a_height;

    m_objects = new Object*[m_objectCount];
    m_oldHeight = new float[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;
        if (obj->GetCollisionShapeType() == CollisionShapeType_Capsule)
        {
            m_oldHeight[i] = ((CapsuleCollisionShape*)obj->GetCollisionShape())->GetHeight();
        }
    }
}
SetCapsuleCollisionShapeHeightAction::~SetCapsuleCollisionShapeHeightAction()
{
    delete[] m_objects;
    delete[] m_oldHeight;
}

e_ActionType SetCapsuleCollisionShapeHeightAction::GetActionType() const
{
    return ActionType_SetCapsuleCollisionShapeHeight;
}

void SetCapsuleCollisionShapeHeightAction::SetData(void* a_data)
{
    m_newHeight = *(float*)a_data;
}

bool SetCapsuleCollisionShapeHeightAction::Redo()
{
    return Execute();
}
bool SetCapsuleCollisionShapeHeightAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        if (obj->GetCollisionObjectType() == CollisionShapeType_Capsule)
        {
            ((CapsuleCollisionShape*)obj->GetCollisionShape())->SetHeight(m_newHeight);
        }
    }

    return true;
}
bool SetCapsuleCollisionShapeHeightAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        if (obj->GetCollisionObjectType() == CollisionShapeType_Capsule)
        {
            ((CapsuleCollisionShape*)obj->GetCollisionShape())->SetHeight(m_oldHeight[i]);
        }
    }

    return true;
}