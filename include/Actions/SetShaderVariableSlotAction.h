#pragma once

#include "Actions/Action.h"

class Object;

class SetShaderVariableSlotAction : public Action
{
private:
    Object**      m_objs;
    unsigned int  m_objectCount;
 
    unsigned int  m_index;

    unsigned int  m_slot;
    unsigned int* m_oldSlot;

protected:

public:
    SetShaderVariableSlotAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, unsigned int a_slot);
    virtual ~SetShaderVariableSlotAction();

    virtual e_ActionType GetActionType() const; 

    inline unsigned int GetIndex() const
    {
        return m_index;
    }
    inline void SetSlot(unsigned int a_slot)
    {
        m_slot = a_slot;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};