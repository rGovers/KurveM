#include "Actions/SetVertexInputVertexSlotAction.h"

#include "Object.h"
#include "ShaderSettings.h"
#include "ShaderVariables/VertexInput.h"

SetVertexInputVertexSlotAction::SetVertexInputVertexSlotAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, e_VertexInputSlot a_slot)
{
    m_index = a_index;
    m_slot = a_slot;

    m_objectCount = a_objectCount;

    m_objs = new Object*[m_objectCount];
    m_oldSlot = new e_VertexInputSlot[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objs[i];
        m_objs[i] = obj;
        m_oldSlot[i] = VertexInputSlot_Null;

        const ShaderSettings* settings = obj->GetShaderSettings();
        if (settings != nullptr)
        {
            const VertexInput* vInput = settings->GetVertexInput(m_index);
            if (vInput != nullptr)
            {
                m_oldSlot[i] = vInput->VertexSlot;
            }
        }
    }
}
SetVertexInputVertexSlotAction::~SetVertexInputVertexSlotAction()
{
    delete[] m_objs;
    delete[] m_oldSlot;
}

e_ActionType SetVertexInputVertexSlotAction::GetActionType() const
{
    return ActionType_SetVertexInputVertexSlot;
}

void SetVertexInputVertexSlotAction::SetData(void* a_data)
{
    m_slot = *(e_VertexInputSlot*)a_data;
}

bool SetVertexInputVertexSlotAction::Redo()
{
    return Execute();
}
bool SetVertexInputVertexSlotAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            VertexInput* input = settings->GetVertexInput(i);
            if (input != nullptr)
            {
                input->VertexSlot = m_slot;
            }
        }
    }

    return true;
}
bool SetVertexInputVertexSlotAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            VertexInput* input = settings->GetVertexInput(i);
            if (input != nullptr)
            {
                input->VertexSlot = m_oldSlot[i];
            }
        }
    }

    return true;
}