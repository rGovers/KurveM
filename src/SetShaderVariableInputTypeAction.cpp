#include "Actions/SetShaderVariableInputTypeAction.h"

#include "Object.h"
#include "ShaderSettings.h"

SetShaderVariableInputTypeAction::SetShaderVariableInputTypeAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, e_VariableInputType a_type)
{
    m_index = a_index;

    m_type = a_type;

    m_objectCount = a_objectCount;

    m_objs = new Object*[m_objectCount];
    m_oldType = new e_VariableInputType[m_objectCount];
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objs[i];
        m_objs[i] = obj;
        m_oldType[i] = VariableInputType_Null;
        const ShaderSettings* settings = obj->GetShaderSettings();
        if (settings != nullptr)
        {
            const ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr)
            {
                m_oldType[i] = var->InputType;
            }
        }
    }
}
SetShaderVariableInputTypeAction::~SetShaderVariableInputTypeAction()
{
    delete[] m_objs;
    delete[] m_oldType;
}

e_ActionType SetShaderVariableInputTypeAction::GetActionType() const
{
    return ActionType_SetShaderVariableInputType;
}

void SetShaderVariableInputTypeAction::SetData(void* a_data)
{
    m_type = *((e_VariableInputType*)a_data);
}

bool SetShaderVariableInputTypeAction::Redo()
{
    return Execute();
}
bool SetShaderVariableInputTypeAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr)
            {
                var->InputType = m_type;
            }
        }
    }

    return true;
}
bool SetShaderVariableInputTypeAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr)
            {
                var->InputType = m_oldType[i];
            }
        }
    }

    return true;
}