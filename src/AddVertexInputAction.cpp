#include "Actions/AddVertexInputAction.h"

#include "Object.h"
#include "ShaderSettings.h"
#include "ShaderVariables/VertexInput.h"

AddVertexInputAction::AddVertexInputAction(Object* const* a_objs, unsigned int a_objectCount)
{
    m_objectCount = a_objectCount;

    m_objs = new Object*[m_objectCount];
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objs[i] = a_objs[i];
    }   

    m_pushed = false;
}
AddVertexInputAction::~AddVertexInputAction()
{
    delete[] m_objs;
}

e_ActionType AddVertexInputAction::GetActionType() const
{
    return ActionType_AddVertexInput;
}

bool AddVertexInputAction::Redo()
{
    return Execute();
}
bool AddVertexInputAction::Execute()
{
    if (!m_pushed)
    {
        for (unsigned int i = 0; i < m_objectCount; ++i)
        {
            ShaderSettings* settings = m_objs[i]->GetShaderSettings();
            settings->PushVertexInput();
        }

        m_pushed = true;

        return true;
    }

    return false;    
}
bool AddVertexInputAction::Revert()
{
    if (m_pushed)
    {
        for (unsigned int i = 0; i < m_objectCount; ++i)
        {
            ShaderSettings* settings = m_objs[i]->GetShaderSettings();
            settings->PopVertexInput();
        }

        m_pushed = false;

        return true;
    }

    return false;
}
