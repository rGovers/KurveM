#include "Actions/SetVertexInputTypeAction.h"

#include "Object.h"
#include "ShaderSettings.h"
#include "ShaderVariables/VertexInput.h"

SetVertexInputTypeAction::SetVertexInputTypeAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, e_VertexInputType a_type)
{
    m_index = a_index;
    m_type = a_type;

    m_objectCount = a_objectCount;

    m_objs = new Object*[m_objectCount];
    m_oldType = new e_VertexInputType[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objs[i];
        m_objs[i] = obj;
        m_oldType[i] = VertexInputType_Null;

        const ShaderSettings* settings = obj->GetShaderSettings();
        if (settings != nullptr)
        {
            const VertexInput* vInput = settings->GetVertexInput(m_index);
            if (vInput != nullptr)
            {
                m_oldType[i] = vInput->Type;
            }
        }
    }
}
SetVertexInputTypeAction::~SetVertexInputTypeAction()
{
    delete[] m_objs;
    delete[] m_oldType;
}

e_ActionType SetVertexInputTypeAction::GetActionType() const
{
    return ActionType_SetVertexInputType;
}

void SetVertexInputTypeAction::SetData(void* a_data)
{
    m_type = *(e_VertexInputType*)a_data;
}

bool SetVertexInputTypeAction::Redo()
{
    return Execute();
}
bool SetVertexInputTypeAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            VertexInput* input = settings->GetVertexInput(m_index);
            if (input != nullptr)
            {
                input->Type = m_type;
            }
        }
    }

    return true;
}
bool SetVertexInputTypeAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            VertexInput* input = settings->GetVertexInput(m_index);
            if (input != nullptr)
            {
                input->Type = m_oldType[i];
            }
        }
    }

    return true;
}