#include "Actions/SetBoxCollisionShapeHalfExtentsAction.h"

#include "Object.h"
#include "Physics/CollisionShapes/BoxCollisionShape.h"

SetBoxCollisionShapeHalfExtentsAction::SetBoxCollisionShapeHalfExtentsAction(Object* const* a_objects, unsigned int a_objectCount, const glm::vec3& a_halfExtents)
{
    m_objectCount = a_objectCount;

    m_newHalfExtents = a_halfExtents;

    m_objects = new Object*[m_objectCount];
    m_oldHalfExtents = new glm::vec3[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;

        if (obj->GetCollisionShapeType() == CollisionShapeType_Box)
        {
            BoxCollisionShape* shape = (BoxCollisionShape*)obj->GetCollisionShape();
            m_oldHalfExtents[i] = shape->GetHalfExtents();
        }
    }
}
SetBoxCollisionShapeHalfExtentsAction::~SetBoxCollisionShapeHalfExtentsAction()
{
    delete[] m_objects;
    delete[] m_oldHalfExtents;
}

e_ActionType SetBoxCollisionShapeHalfExtentsAction::GetActionType() const
{
    return ActionType_SetBoxCollisionShapeHalfExtents;
}

void SetBoxCollisionShapeHalfExtentsAction::SetData(void* a_data)
{
    m_newHalfExtents = *(glm::vec3*)a_data;
}

bool SetBoxCollisionShapeHalfExtentsAction::Redo()
{
    return Execute();
}
bool SetBoxCollisionShapeHalfExtentsAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionShapeType() == CollisionShapeType_Box)
        {
            ((BoxCollisionShape*)obj->GetCollisionShape())->SetHalfExtents(m_newHalfExtents);
        }
    }

    return true;
}
bool SetBoxCollisionShapeHalfExtentsAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionShapeType() == CollisionShapeType_Box)
        {
            ((BoxCollisionShape*)obj->GetCollisionShape())->SetHalfExtents(m_oldHalfExtents[i]);
        }
    }

    return true;
}