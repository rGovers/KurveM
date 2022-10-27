#pragma once

#include "Actions/Action.h"

#include "ShaderVariables/VertexInput.h"

class Object;

class SetVertexInputVertexSlotAction : public Action
{
private:
    Object**           m_objs;
    unsigned int       m_objectCount;
      
    unsigned int       m_index;

    e_VertexInputSlot  m_slot;
    e_VertexInputSlot* m_oldSlot;

protected:

public:
    SetVertexInputVertexSlotAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, e_VertexInputSlot a_slot);
    virtual ~SetVertexInputVertexSlotAction();

    virtual e_ActionType GetActionType() const; 

    inline unsigned int GetIndex() const
    {
        return m_index;
    }
    inline void SetSlot(e_VertexInputSlot a_slot)
    {
        m_slot = a_slot;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};