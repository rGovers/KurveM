#include "Actions/SetSphereCollisionShapeRadiusAction.h"

#include "Object.h"
#include "Physics/CollisionShapes/SphereCollisionShape.h"

SetSphereCollisionShapeRadiusAction::SetSphereCollisionShapeRadiusAction(Object* const* a_objects, unsigned int a_objectCount, float a_radius)
{
    m_objectCount = a_objectCount;

    m_newRadius = a_radius;

    m_objects = new Object*[m_objectCount];
    m_oldRadius = new float[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;
        if (obj->GetCollisionShapeType() == CollisionShapeType_Sphere)
        {
            SphereCollisionShape* shape = (SphereCollisionShape*)obj->GetCollisionShape();
            m_oldRadius[i] = shape->GetRadius();
        }
    }
}
SetSphereCollisionShapeRadiusAction::~SetSphereCollisionShapeRadiusAction()
{
    delete[] m_oldRadius;
    delete[] m_objects;
}

e_ActionType SetSphereCollisionShapeRadiusAction::GetActionType() const
{
    return ActionType_SetSphereCollisionShapeRadius;
}

void SetSphereCollisionShapeRadiusAction::SetData(void* a_data)
{
    m_newRadius = *(float*)a_data;
}

bool SetSphereCollisionShapeRadiusAction::Redo()
{
    return Execute();

}
bool SetSphereCollisionShapeRadiusAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        if (obj->GetCollisionShapeType() == CollisionShapeType_Sphere)
        {
            SphereCollisionShape* shape = (SphereCollisionShape*)obj->GetCollisionShape();
            shape->SetRadius(m_newRadius);
        }
    }

    return true;
}
bool SetSphereCollisionShapeRadiusAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        if (obj->GetCollisionShapeType() == CollisionShapeType_Sphere)
        {
            SphereCollisionShape* shape = (SphereCollisionShape*)obj->GetCollisionShape();
            shape->SetRadius(m_oldRadius[i]);
        }
    }

    return true;
}