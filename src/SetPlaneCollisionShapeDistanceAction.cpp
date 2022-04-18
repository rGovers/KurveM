#include "Actions/SetPlaneCollisionShapeDistanceAction.h"

#include "Object.h"
#include "Physics/CollisionShapes/PlaneCollisionShape.h"

SetPlaneCollisionShapeDistanceAction::SetPlaneCollisionShapeDistanceAction(Object* const* a_objects, unsigned int a_objectCount, float a_distance)
{
    m_objectCount = a_objectCount;

    m_newDist = a_distance;

    m_objects = new Object*[m_objectCount];
    m_oldDist = new float[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;

        if (obj->GetCollisionShapeType() == CollisionShapeType_Plane)
        {
            m_oldDist[i] = ((PlaneCollisionShape*)obj->GetCollisionShape())->GetDistance();
        }
    }
}
SetPlaneCollisionShapeDistanceAction::~SetPlaneCollisionShapeDistanceAction()
{
    delete[] m_objects;
    delete[] m_oldDist;
}

e_ActionType SetPlaneCollisionShapeDistanceAction::GetActionType()
{
    return ActionType_SetPlaneCollisionShapeDistance;
}

bool SetPlaneCollisionShapeDistanceAction::Redo()
{
    return Execute();
}
bool SetPlaneCollisionShapeDistanceAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionShapeType() == CollisionShapeType_Plane)
        {
            PlaneCollisionShape* shape = (PlaneCollisionShape*)obj->GetCollisionShape();
            shape->SetDistance(m_newDist);

            CollisionObject* cObj = obj->GetCollisionObject();
            if (cObj != nullptr)
            {
                cObj->SetCollisionShape(shape);
            }
        }
    }

    return true;
}
bool SetPlaneCollisionShapeDistanceAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];

        if (obj->GetCollisionShapeType() == CollisionShapeType_Plane)
        {
            PlaneCollisionShape* shape = (PlaneCollisionShape*)obj->GetCollisionShape();
            shape->SetDistance(m_oldDist[i]);

            CollisionObject* cObj = obj->GetCollisionObject();
            if (cObj != nullptr)
            {
                cObj->SetCollisionShape(shape);
            }
        }
    }

    return true;
}