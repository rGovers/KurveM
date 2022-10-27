#include "Actions/SetShaderVariableTypeAction.h"

#include "Object.h"
#include "ShaderSettings.h"

SetShaderVariableTypeAction::SetShaderVariableTypeAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, e_VariableType a_type)
{
    m_index = a_index;

    m_type = a_type;

    m_objectCount = a_objectCount;
    m_objs = new Object*[m_objectCount];
    m_oldType = new e_VariableType[m_objectCount];
    m_oldData = new void*[m_objectCount];
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objs[i];
        m_objs[i] = obj;
        m_oldData[i] = nullptr;
        m_oldType[i] = VariableType_Null;

        const ShaderSettings* settings = obj->GetShaderSettings();
        if (settings != nullptr)
        {
            const ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr)
            {
                m_oldType[i] = var->Type;
            }
        }
    } 
}
SetShaderVariableTypeAction::~SetShaderVariableTypeAction()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        if (m_oldData[i] != nullptr)
        {
            switch (m_oldType[i])
            {
            case VariableType_Mat3:
            {
                delete (glm::mat3*)(m_oldData[i]);

                break;
            }
            case VariableType_Mat4:
            {
                delete (glm::mat4*)(m_oldData[i]);

                break;
            }
            }
        }
    }

    delete[] m_objs;
    delete[] m_oldData;
    delete[] m_oldType;
}

e_ActionType SetShaderVariableTypeAction::GetActionType() const
{
    return ActionType_SetShaderVariableType;
}

void SetShaderVariableTypeAction::SetData(void* a_data)
{
    m_type = *((e_VariableType*)a_data);
}

bool SetShaderVariableTypeAction::Redo()
{
    return Execute();
}
bool SetShaderVariableTypeAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr)
            {
                var->Type = m_type;
                if (m_oldData[i] == nullptr)
                {
                    m_oldData[i] = var->Variable;
                    switch (m_type)
                    {
                    case VariableType_Mat3:
                    {
                        var->Variable = new glm::mat3(1.0f);

                        break;
                    }
                    case VariableType_Mat4:
                    {
                        var->Variable = new glm::mat4(1.0f);

                        break;
                    }
                    default:
                    {
                        var->Variable = nullptr;

                        break;
                    }
                    }
                }
            }
        }
    }

    return true;
}
bool SetShaderVariableTypeAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr)
            {
                if (var->Variable != nullptr)
                {
                    switch (m_type)
                    {
                    case VariableType_Mat3:
                    {
                        delete (glm::mat3*)var->Variable;

                        break;
                    }
                    case VariableType_Mat4:
                    {
                        delete (glm::mat4*)var->Variable;

                        break;
                    }
                    }
                }

                var->Type = m_oldType[i];
                var->Variable = m_oldData[i];
                m_oldData[i] = nullptr;
            }
        }
    }

    return true;
}