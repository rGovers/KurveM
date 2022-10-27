#include "Actions/SetVertexShaderAction.h"

#include <string.h>

#include "Object.h"
#include "ShaderSettings.h"

SetVertexShaderAction::SetVertexShaderAction(Workspace* a_workspace, Object* const* a_objects, unsigned int a_objectCount, const char* a_newShader)
{
    m_workspace = a_workspace;

    m_objectCount = a_objectCount;

    m_objects = new Object*[m_objectCount];
    m_oldShaders = new char*[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];

        m_objects[i] = obj;

        const ShaderSettings* settings = obj->GetShaderSettings();
        const char* shaderPath = settings->GetVertexShaderPath();

        m_oldShaders[i] = nullptr;
        if (shaderPath != nullptr)
        {
            const int pathLen = (int)strlen(shaderPath) + 1;

            m_oldShaders[i] = new char[pathLen];
            for (int j = 0; j < pathLen; ++j)
            {
                m_oldShaders[i][j] = shaderPath[j];
            }
        }
    }

    m_newShader = nullptr;
    if (a_newShader != nullptr)
    {
        const int pathLen = (int)strlen(a_newShader) + 1;

        m_newShader = new char[pathLen];
        for (int i = 0; i < pathLen; ++i)
        {
            m_newShader[i] = a_newShader[i];
        }
    }
}
SetVertexShaderAction::~SetVertexShaderAction()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        if (m_oldShaders[i] != nullptr)
        {
            delete[] m_oldShaders[i];
            m_oldShaders[i] = nullptr;
        }
    }

    delete[] m_objects;
    delete[] m_oldShaders;

    if (m_newShader != nullptr)
    {
        delete[] m_newShader;
        m_newShader = nullptr;
    }
}

e_ActionType SetVertexShaderAction::GetActionType() const
{
    return ActionType_SetVertexShader;    
}

bool SetVertexShaderAction::Redo()
{
    return Execute();
}
bool SetVertexShaderAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        ShaderSettings* settings = m_objects[i]->GetShaderSettings();

        settings->SetVertexShaderPath(m_newShader);
    }

    return true;
}
bool SetVertexShaderAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        ShaderSettings* settings = m_objects[i]->GetShaderSettings();

        settings->SetVertexShaderPath(m_oldShaders[i]);
    }

    return true;
}