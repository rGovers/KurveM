#pragma once

#include "Actions/Action.h"

class Object;

class SetCapsuleCollisionShapeHeightAction : public Action
{
private:
    unsigned int m_objectCount;

    Object**     m_objects;
    
    float*       m_oldHeight;
    float        m_newHeight;

protected:

public:
    SetCapsuleCollisionShapeHeightAction(Object* const* a_objects, unsigned int a_objectCount, float a_height);
    virtual ~SetCapsuleCollisionShapeHeightAction();

    virtual e_ActionType GetActionType() const;

    inline void SetHeight(float a_value)
    {
        m_newHeight = a_value;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};