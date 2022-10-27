#include "Actions/SetCollisionObjectTypeAction.h"

#include "Object.h"
#include "Physics/CollisionObjects/Rigidbody.h"
#include "Physics/CollisionObjects/Softbody.h"

SetCollisionObjectTypeAction::SetCollisionObjectTypeAction(e_CollisionObjectType a_type, Object* const* a_objs, unsigned int a_objectCount, Workspace* a_workspace, PhysicsEngine* a_engine)
{
    m_workspace = a_workspace;
    m_engine = a_engine;

    m_objectCount = a_objectCount;
    m_objs = new Object*[m_objectCount];
    m_oldCObjs = new CollisionObject*[m_objectCount];
    m_state = new unsigned char[m_objectCount];

    m_type = a_type;

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objs[i]; 
        m_objs[i] = obj;
        
        m_state[i] = 0;

        CollisionObject* cObj = obj->GetCollisionObject();
        m_oldCObjs[i] = cObj; 
        if (cObj != nullptr)
        {
            if (cObj->IsActive())
            {
                m_state[i] |= 0b1 << 0;
            }
        }
        else
        {
            m_state[i] |= 0b1 << 0;
        }
    }
}
SetCollisionObjectTypeAction::~SetCollisionObjectTypeAction()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        if (m_state[i] & (0b1 << 1))
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

e_ActionType SetCollisionObjectTypeAction::GetActionType() const
{
    return ActionType_SetCollisionObjectType;
}

void SetCollisionObjectTypeAction::SetData(void* a_data)
{
    m_type = *(e_CollisionObjectType*)a_data;
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

        if (m_state[i] & (0b1 << 1))
        {
            delete obj->GetCollisionObject();
        }
        
        if (m_oldCObjs[i] != nullptr && m_type == m_oldCObjs[i]->GetCollisionObjectType())
        {
            CollisionObject* cObj = m_oldCObjs[i];

            cObj->SetActiveState(m_state[i] & (0b1 << 0));
            cObj->SetCollisionShape(obj->GetCollisionShape());
            obj->SetCollisionObject(cObj);

            m_state[i] &= ~(0b1 << 1);
        }
        else
        {   
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
                if (obj->GetObjectType() != ObjectType_ArmatureNode)
                {
                    cObj = new Rigidbody(obj, m_engine);
                }

                break;
            }
            case CollisionObjectType_Softbody:
            {
                switch (obj->GetObjectType())
                {
                case ObjectType_CurveModel:
                case ObjectType_PathModel:
                {
                    cObj = new Softbody(obj, m_workspace, m_engine);

                    break;
                }
                }

                break;
            }
            }

            if (cObj != nullptr)
            {
                cObj->SetCollisionShape(obj->GetCollisionShape());
                cObj->SetActiveState(m_state[i] & (0b1 << 0));
            }

            obj->SetCollisionObject(cObj);

            m_state[i] |= 0b1 << 1;
        }
    }

    return true;
}
bool SetCollisionObjectTypeAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objs[i];
        
        if (m_state[i] & (0b1 << 1))
        {
            delete obj->GetCollisionObject();
        }

        CollisionObject* cObj = m_oldCObjs[i];

        if (cObj != nullptr)
        {
            cObj->SetActiveState(m_state[i] & (0b1 << 0));
        }
        obj->SetCollisionObject(cObj);

        m_state[i] &= (0b1 << 1);
    }

    return true;
}