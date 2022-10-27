#include "Actions/SetPixelShaderAction.h"

#include <string.h>

#include "Object.h"
#include "ShaderSettings.h"

SetPixelShaderAction::SetPixelShaderAction(Workspace* a_workspace, Object* const* a_objects, unsigned int a_objectCount, const char* a_newShader)
{
    m_workspace = a_workspace;

    m_objectCount = a_objectCount;

    m_objects = new Object*[m_objectCount];
    m_oldShaders = new char*[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];

        m_objects[i] = obj;
        m_oldShaders[i] = nullptr;

        const ShaderSettings* settings = obj->GetShaderSettings();
        const char* path = settings->GetPixelShaderPath();
        if (path != nullptr)
        {
            const int len = (int)strlen(path) + 1;

            m_oldShaders[i] = new char[len];
            for (int j = 0; j < len; ++j)
            {
                m_oldShaders[i][j] = path[j];
            }
        }
    }

    m_newShader = nullptr;
    if (a_newShader != nullptr)
    {
        const int len = (int)strlen(a_newShader) + 1;

        m_newShader = new char[len];
        for (int i = 0; i < len; ++i)
        {
            m_newShader[i] = a_newShader[i];
        }
    }
}
SetPixelShaderAction::~SetPixelShaderAction()
{
    for (int i = 0; i < m_objectCount; ++i)
    {
        if (m_oldShaders[i] != nullptr)
        {
            delete[] m_oldShaders[i];
            m_oldShaders[i] = nullptr;
        }
    }

    delete[] m_oldShaders;
    delete[] m_objects;

    if (m_newShader != nullptr)
    {
        delete[] m_newShader;
        m_newShader = nullptr;
    }
}

e_ActionType SetPixelShaderAction::GetActionType() const
{
    return ActionType_SetPixelShader;
}

bool SetPixelShaderAction::Redo()
{
    return Execute();
}
bool SetPixelShaderAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        ShaderSettings* settings = obj->GetShaderSettings();

        settings->SetPixelShaderPath(m_newShader);
    }

    return true;
}
bool SetPixelShaderAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = m_objects[i];
        ShaderSettings* settings = obj->GetShaderSettings();

        settings->SetPixelShaderPath(m_oldShaders[i]);
    }

    return true;
}