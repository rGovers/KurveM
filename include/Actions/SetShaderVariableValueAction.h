#pragma once

#include "Actions/Action.h"

#include "ShaderVariables/ShaderVariable.h"

class Object;

class SetShaderVariableValueAction : public Action
{
private:
    Object**        m_objs;
    unsigned int    m_objectCount;

    unsigned int    m_index;

    e_VariableType m_type;

    void*          m_value;
    void**         m_oldValues;

protected:

public:
    SetShaderVariableValueAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, e_VariableType a_type, void* a_value);
    virtual ~SetShaderVariableValueAction();

    virtual e_ActionType GetActionType() const; 

    inline unsigned int GetIndex() const
    {
        return m_index;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};