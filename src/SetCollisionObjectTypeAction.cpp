#include "Actions/SetCollisionObjectTypeAction.h"

#include "Object.h"
#include "Physics/CollisionObjects/Rigidbody.h"
#include "Physics/CollisionObjects/Softbody.h"

SetCollisionObjectTypeAction::SetCollisionObjectTypeAction(e_CollisionObjectType a_type, Object* const* a_objs, unsigned int a_objectCount, PhysicsEngine* a_engine)
{
    m_engine = a_engine;

    m_objectCount = a_objectCount;
    m_objs = new Object*[m_objectCount];
    m_oldCObjs = new CollisionObject*[m_objectCount];
    m_state = new bool[m_objectCount];

    m_type = a_type;

    m_own = false;
    
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objs[i]; 
        m_objs[i] = obj;
        CollisionObject* cObj = obj->GetCollisionObject();
        m_oldCObjs[i] = cObj; 
        if (cObj != nullptr)
        {
            m_state[i] = cObj->IsActive();
        }
    }
}
SetCollisionObjectTypeAction::~SetCollisionObjectTypeAction()
{
    if (m_own)
    {
        for (unsigned int i = 0; i < m_objectCount; ++i)
        {
            if (m_oldCObjs[i] != nullptr)
            {
                delete m_oldCObjs[i];
            }
        }
    }

    delete[] m_state;
    delete[] m_objs;
    delete[] m_oldCObjs;
}

e_ActionType SetCollisionObjectTypeAction::GetActionType()
{
    return ActionType_SetCollisionObjectType;
}

bool SetCollisionObjectTypeAction::Redo()
{
    return Execute();
}
bool SetCollisionObjectTypeAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        CollisionObject* oldCObj = m_oldCObjs[i];
        if (oldCObj != nullptr)
        {
            oldCObj->SetActiveState(false);
        }

        Object* obj = m_objs[i];

        if (m_own)
        {
            delete obj->GetCollisionObject();
        }

        CollisionObject* cObj = nullptr;

        switch (m_type)
        {
        case CollisionObjectType_CollisionObject:
        {
            cObj = new CollisionObject(obj, m_engine);

            break;
        }
        case CollisionObjectType_Rigidbody:
        {
            cObj = new Rigidbody(obj, m_engine);

            break;
        }
        case CollisionObjectType_Softbody:
        {
            cObj = new Softbody(obj, m_engine);

            break;
        }
        }

        if (cObj != nullptr)
        {
            cObj->SetCollisionShape(obj->GetCollisionShape());
        }

        obj->SetCollisionObject(cObj);
    }

    m_own = true;

    return true;
}
bool SetCollisionObjectTypeAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objs[i];
        
        if (m_own)
        {
            delete obj->GetCollisionObject();
        }

        CollisionObject* cObj = m_oldCObjs[i];

        if (cObj != nullptr)
        {
            cObj->SetActiveState(m_state[i]);
        }
        obj->SetCollisionObject(cObj);
    }

    m_own = false;

    return true;
}