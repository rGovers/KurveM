#include "Actions/SetPlaneCollisionShapeDirectionAction.h"

#include "Object.h"
#include "Physics/CollisionShapes/PlaneCollisionShape.h"

SetPlaneCollisionShapeDirectionAction::SetPlaneCollisionShapeDirectionAction(Object* const* a_objects, unsigned int a_objectCount, const glm::vec3& a_dir)
{
    m_objectCount = a_objectCount;

    m_newDir = a_dir;

    m_objects = new Object*[m_objectCount];
    m_oldDir = new glm::vec3[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;
        if (obj->GetCollisionShapeType() == CollisionShapeType_Plane)
        {
            m_oldDir[i] = ((PlaneCollisionShape*)obj->GetCollisionShape())->GetDirection();
        }
    }
}
SetPlaneCollisionShapeDirectionAction::~SetPlaneCollisionShapeDirectionAction()
{
    delete[] m_objects;
    delete[] m_oldDir;
}

e_ActionType SetPlaneCollisionShapeDirectionAction::GetActionType()
{
    return ActionType_SetPlaneCollisionShapeDirection;
}

bool SetPlaneCollisionShapeDirectionAction::Redo()
{
    return Execute();
}
bool SetPlaneCollisionShapeDirectionAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        if (obj->GetCollisionShapeType() == CollisionShapeType_Plane)
        {
            PlaneCollisionShape* shape = (PlaneCollisionShape*)obj->GetCollisionShape();
            shape->SetDirection(m_newDir);

            CollisionObject* cObj = obj->GetCollisionObject();
            if (cObj != nullptr)
            {
                cObj->SetCollisionShape(shape);
            }
        }
    }

    return true;
}
bool SetPlaneCollisionShapeDirectionAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        if (obj->GetCollisionShapeType() == CollisionShapeType_Plane)
        {
            PlaneCollisionShape* shape = (PlaneCollisionShape*)obj->GetCollisionShape();
            shape->SetDirection(m_oldDir[i]);

            CollisionObject* cObj = obj->GetCollisionObject();
            if (cObj != nullptr)
            {
                cObj->SetCollisionShape(shape);
            }
        }
    }

    return true;
}