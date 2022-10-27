#pragma once

#include "Actions/Action.h"

#include "ShaderVariables/ShaderVariable.h"

class Object;

class SetShaderVariableInputTypeAction : public Action
{
private:
    Object**             m_objs;
    unsigned int         m_objectCount;
         
    unsigned int         m_index;

    e_VariableInputType  m_type;
    e_VariableInputType* m_oldType;

protected:

public:
    SetShaderVariableInputTypeAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, e_VariableInputType a_type);
    virtual ~SetShaderVariableInputTypeAction();

    virtual e_ActionType GetActionType() const; 

    inline unsigned int GetIndex() const
    {
        return m_index;
    }
    inline void SetType(e_VariableInputType a_type)
    {
        m_type = a_type;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};