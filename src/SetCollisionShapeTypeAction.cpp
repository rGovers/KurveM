#include "Actions/SetCollisionShapeTypeAction.h"

#include "Object.h"
#include "Physics/CollisionShapes/SphereCollisionShape.h"

SetCollisionShapeTypeAction::SetCollisionShapeTypeAction(e_CollisionShapeType a_type, Object* const* a_objs, unsigned int a_objectCount)
{
    m_type = a_type;
    
    m_objectCount = a_objectCount;

    m_own = false;

    m_objs = new Object*[m_objectCount];
    m_oldShapes = new CollisionShape*[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objs[i];
        m_objs[i] = obj;
        m_oldShapes[i] = obj->GetCollisionShape();
    }
}
SetCollisionShapeTypeAction::~SetCollisionShapeTypeAction()
{
    if (m_own)
    {
        for (unsigned int i = 0; i < m_objectCount; ++i)
        {
            if (m_oldShapes[i] != nullptr)
            {
                delete m_oldShapes[i];
            }
        }
    }

    delete[] m_objs;
    delete[] m_oldShapes;
}

e_ActionType SetCollisionShapeTypeAction::GetActionType()
{
    return ActionType_SetCollisionShapeType;
}

bool SetCollisionShapeTypeAction::Redo()
{
    return Execute();
}
bool SetCollisionShapeTypeAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objs[i];

        if (m_own)
        {
            delete obj->GetCollisionShape();
        }

        CollisionShape* shape = nullptr;
        switch (m_type)
        {
        case CollisionShapeType_Sphere:
        {
            shape = new SphereCollisionShape(1.0f);

            break;
        }
        }

        obj->SetCollisionShape(shape);
        CollisionObject* cObj = obj->GetCollisionObject();
        if (cObj != nullptr)
        {
            cObj->SetCollisionShape(shape);
        }
    }

    m_own = true;

    return true;
}
bool SetCollisionShapeTypeAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objs[i];

        if (m_own)
        {
            delete obj->GetCollisionShape();
        }

        CollisionShape* shape = m_oldShapes[i];

        obj->SetCollisionShape(shape);
        CollisionObject* cObj = obj->GetCollisionObject();
        if (cObj != nullptr)
        {
            cObj->SetCollisionShape(shape);
        }
    }

    m_own = false;

    return true;
}