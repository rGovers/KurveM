#include "Actions/AddShaderVariableAction.h"

#include "Object.h"
#include "ShaderSettings.h"

AddShaderVariableAction::AddShaderVariableAction(Object* const* a_objs, unsigned int a_objectCount)
{
    m_objectCount = a_objectCount;

    m_objs = new Object*[m_objectCount];
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objs[i] = a_objs[i];
    }

    m_pushed = false;
}
AddShaderVariableAction::~AddShaderVariableAction()
{
    delete[] m_objs;
}

e_ActionType AddShaderVariableAction::GetActionType() const
{
    return ActionType_AddShaderVariable;
}

bool AddShaderVariableAction::Redo() 
{
    return Execute();
}
bool AddShaderVariableAction::Execute()
{
    if (!m_pushed)
    {
        for (unsigned int i = 0; i < m_objectCount; ++i)
        {
            ShaderSettings* settings = m_objs[i]->GetShaderSettings();
            settings->PushShaderVariable();
        }

        m_pushed = true;

        return true;
    }

    return false;
}
bool AddShaderVariableAction::Revert()
{
    if (!m_pushed)
    {
        for (unsigned int i = 0; i < m_objectCount; ++i)
        {
            ShaderSettings* settings = m_objs[i]->GetShaderSettings();
            settings->PopShaderVariable();
        }

        m_pushed = false;

        return true;
    }

    return false;
}