#include "Actions/SetShaderVariableSlotAction.h"

#include "Object.h"
#include "ShaderSettings.h"
#include "ShaderVariables/ShaderVariable.h"

SetShaderVariableSlotAction::SetShaderVariableSlotAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, unsigned int a_slot)
{
    m_slot = a_slot;

    m_index = a_index;

    m_objectCount = a_objectCount;
    m_objs = new Object*[m_objectCount];
    m_oldSlot = new unsigned int[m_objectCount];
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objs[i];
        m_objs[i] = obj;
        m_oldSlot[i] = -1;

        const ShaderSettings* settings = obj->GetShaderSettings();
        if (settings != nullptr)
        {
            const ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr)
            {
                m_oldSlot[i] = var->Slot;
            }
        }
    }
}
SetShaderVariableSlotAction::~SetShaderVariableSlotAction()
{
    delete[] m_objs;
    delete[] m_oldSlot;
}

e_ActionType SetShaderVariableSlotAction::GetActionType() const
{
    return ActionType_SetShaderVariableSlot;
}

void SetShaderVariableSlotAction::SetData(void* a_data)
{
    m_slot = *((unsigned int*)a_data);
}

bool SetShaderVariableSlotAction::Redo()
{
    return Execute();
}
bool SetShaderVariableSlotAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr)
            {
                var->Slot = m_slot;
            }
        }
    }

    return true;
}
bool SetShaderVariableSlotAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr)
            {
                var->Slot = m_oldSlot[i];
            }
        }
    }

    return true;
}