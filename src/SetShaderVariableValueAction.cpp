#include "Actions/SetShaderVariableValueAction.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Object.h"
#include "ShaderSettings.h"

SetShaderVariableValueAction::SetShaderVariableValueAction(Object* const* a_objs, unsigned int a_objectCount, unsigned int a_index, e_VariableType a_type, void* a_value)
{
    m_index = a_index;

    m_type = a_type;

    switch (m_type)
    {
    case VariableType_Mat3:
    {
        m_value = new glm::mat3(*(glm::mat3*)a_value);

        break;
    }
    case VariableType_Mat4:
    {
        m_value = new glm::mat4(*(glm::mat4*)a_value);

        break;
    }
    default:
    {
        m_value = nullptr;

        break;
    }
    }

    m_objectCount = a_objectCount;
    m_objs = new Object*[m_objectCount];
    m_oldValues = new void*[m_objectCount];
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objs[i] = a_objs[i];
        m_oldValues[i] = nullptr;
    }
}
SetShaderVariableValueAction::~SetShaderVariableValueAction()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        if (m_oldValues[i] != nullptr)
        {
            switch (m_type)
            {
            case VariableType_Mat3:
            {
                delete (glm::mat3*)m_oldValues[i];

                break;
            }
            case VariableType_Mat4:
            {
                delete (glm::mat4*)m_oldValues[i];

                break;
            }
            }
        }
    }

    if (m_value != nullptr)
    {
        switch (m_type)
        {
        case VariableType_Mat3:
        {
            delete (glm::mat3*)m_value;

            break;
        }
        case VariableType_Mat4:
        {
            delete (glm::mat4*)m_value;

            break;
        }
        }
    }

    delete[] m_oldValues;
    delete[] m_objs;
}

e_ActionType SetShaderVariableValueAction::GetActionType() const
{
    return ActionType_SetShaderVariableValue;
}

void SetShaderVariableValueAction::SetData(void* a_data)
{
    switch (m_type)
    {
    case VariableType_Mat3:
    {
        *(glm::mat3*)m_value = *(glm::mat3*)a_data;

        return;
    }
    case VariableType_Mat4:
    {
        *(glm::mat4*)m_value = *(glm::mat4*)a_data;
    }
    }
}

bool SetShaderVariableValueAction::Redo()
{
    return Execute();
}
bool SetShaderVariableValueAction::Execute()
{
    if (m_value == nullptr)
    {
        return false;
    }

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
        if (settings != nullptr)
        {
            ShaderVariable* var = settings->GetShaderVariable(m_index);
            if (var != nullptr && var->Type == m_type)
            {
                if (m_oldValues[i] == nullptr)
                {
                    m_oldValues[i] = var->Variable;
                    var->Variable = nullptr;
                }

                switch (m_type)
                {
                case VariableType_Mat3:
                {
                    if (var->Variable == nullptr)
                    {
                        var->Variable = new glm::mat3(*(glm::mat3*)m_value);
                    }
                    else
                    {
                        *(glm::mat3*)var->Variable = *(glm::mat3*)m_value;
                    }

                    break;
                }
                case VariableType_Mat4:
                {
                    if (var->Variable == nullptr)
                    {
                        var->Variable = new glm::mat4(*(glm::mat4*)m_value);
                    }
                    else
                    {
                        *(glm::mat4*)var->Variable = *(glm::mat4*)m_value;
                    }

                    break;
                }
                } 
            }
        }
    }

    return true;
}
bool SetShaderVariableValueAction::Revert()
{
    if (m_value == nullptr)
    {
        return false;
    }

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        if (m_oldValues[i] != nullptr)
        {
            const ShaderSettings* settings = m_objs[i]->GetShaderSettings();
            if (settings != nullptr)
            {
                ShaderVariable* var = settings->GetShaderVariable(m_index);
                if (var != nullptr && var->Type == m_type)
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

                    var->Variable = m_oldValues[i];
                    m_oldValues[i] = nullptr;
                }
            }
        }
    } 

    return true;
}