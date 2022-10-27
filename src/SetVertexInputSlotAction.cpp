#include "Actions/SetVertexInputSlotAction.h"

#include "Object.h"
#include "ShaderSettings.h"
#include "ShaderVariables/VertexInput.h"

SetVertexInputSlotAction::SetVertexInputSlotAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, unsigned int a_slot)
{
    m_index = a_index;
    m_slot = a_slot;

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
            const VertexInput* vInput = settings->GetVertexInput(m_index);
            if (vInput != nullptr)
            {
                m_oldSlot[i] = vInput->Slot;
            }
        }
    }
}
SetVertexInputSlotAction::~SetVertexInputSlotAction()
{
    delete[] m_objs;
    delete[] m_oldSlot;
}

e_ActionType SetVertexInputSlotAction::GetActionType() const
{
    return ActionType_SetVertexInputSlot;
}

void SetVertexInputSlotAction::SetData(void* a_data)
{
    m_slot = *(unsigned int*)a_data;
}

bool SetVertexInputSlotAction::Redo()
{
    return Execute();
}
bool SetVertexInputSlotAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            VertexInput* input = settings->GetVertexInput(m_index);
            if (input != nullptr)
            {
                input->Slot = m_slot;
            }
        }
    }

    return true;
}
bool SetVertexInputSlotAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            VertexInput* input = settings->GetVertexInput(m_index);
            if (input != nullptr)
            {
                input->Slot = m_oldSlot[i];
            }
        }
    }

    return true;
}