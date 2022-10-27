#pragma once

#include "Actions/Action.h"

#include "ShaderVariables/VertexInput.h"

class Object;

class SetVertexInputTypeAction : public Action
{
private:
    Object**           m_objs;
    unsigned int       m_objectCount;
 
    unsigned int       m_index;

    e_VertexInputType  m_type;
    e_VertexInputType* m_oldType;

protected:

public:
    SetVertexInputTypeAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, e_VertexInputType a_type);
    virtual ~SetVertexInputTypeAction();

    virtual e_ActionType GetActionType() const; 

    inline unsigned int GetIndex() const
    {
        return m_index;
    }
    inline void SetType(e_VertexInputType a_type)
    {
        m_type = a_type;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};