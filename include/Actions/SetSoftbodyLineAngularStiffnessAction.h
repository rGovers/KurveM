#pragma once

#include "Actions/Action.h"

class Object;

class SetSoftbodyLineAngularStiffnessAction : public Action
{
private:
    unsigned int m_objectCount;

    Object**     m_objects;
    
    float*       m_oldStiffness;
    float        m_newStiffness;

protected:

public:
    SetSoftbodyLineAngularStiffnessAction(Object* const* a_objects, unsigned int a_objectCount, float a_stiffness);
    virtual ~SetSoftbodyLineAngularStiffnessAction();

    virtual e_ActionType GetActionType() const;

    inline void SetStiffness(float a_value)
    {
        m_newStiffness = a_value;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};