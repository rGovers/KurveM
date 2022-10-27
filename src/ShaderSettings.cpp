#include "ShaderSettings.h"

#include "Datastore.h"
#include "ShaderProgram.h"
#include "ShaderVariables/ShaderVariable.h"
#include "ShaderVariables/VertexInput.h"

#include <string.h>

ShaderSettings::ShaderSettings()
{
    m_vertexShaderPath = nullptr;
    m_pixelShaderPath = nullptr;

    const char* name = GetShaderProgramName();

    m_shaderProgram = Datastore::GetShaderProgram(name);

    delete[] name;
}
ShaderSettings::~ShaderSettings()
{
    for (auto iter = m_vertexInputs.begin(); iter != m_vertexInputs.end(); ++iter)
    {
        delete *iter;
    }

    for (auto iter = m_shaderVars.begin(); iter != m_shaderVars.end(); ++iter)
    {
        if ((*iter)->Variable != nullptr)
        {
            delete (*iter)->Variable;
        }
        
        delete *iter;
    }

    if (m_vertexShaderPath != nullptr)
    {
        delete[] m_vertexShaderPath;
        m_vertexShaderPath = nullptr;
    }

    if (m_pixelShaderPath != nullptr)
    {
        delete[] m_pixelShaderPath;
        m_pixelShaderPath = nullptr;
    }
}

char* ShaderSettings::GetShaderProgramName() const
{
    const char* vertPath = m_vertexShaderPath;
    const char* pixelPath = m_pixelShaderPath;

    if (vertPath == nullptr)
    {
        vertPath = ShaderProgram::BaseShaderName;
    }
    if (pixelPath == nullptr)
    {
        pixelPath = ShaderProgram::BaseShaderName;
    }

    const int verLen = (int)strlen(vertPath);
    const int pixLen = (int)strlen(pixelPath);

    const int len = verLen + pixLen + 6;
    char* name = new char[len] { 0 };

    name[0] = '[';
    for (int i = 0; i < verLen; ++i)
    {
        name[i + 1] = vertPath[i];
    }

    name[verLen + 1] = ']';
    name[verLen + 2] = ' ';
    name[verLen + 3] = '[';

    for (int i = 0; i < pixLen; ++i)
    {
        name[i + verLen + 4] = pixelPath[i];
    }

    name[verLen + pixLen + 4] = ']';
    name[verLen + pixLen + 5] = 0;

    return name;
}

void ShaderSettings::SetVertexShaderPath(const char* a_path)
{
    if (m_vertexShaderPath != nullptr)
    {
        delete[] m_vertexShaderPath;
        m_vertexShaderPath = nullptr;
    }

    if (a_path != nullptr)
    {
        const int len = (int)strlen(a_path) + 1;

        m_vertexShaderPath = new char[len];

        for (int i = 0; i < len; ++i)
        {
            m_vertexShaderPath[i] = a_path[i];
        }
    }

    const char* name = GetShaderProgramName();

    m_shaderProgram = Datastore::LoadShaderProgram(m_vertexShaderPath, m_pixelShaderPath);

    delete[] name;
}
void ShaderSettings::SetPixelShaderPath(const char* a_path)
{
    if (m_pixelShaderPath != nullptr)
    {
        delete[] m_pixelShaderPath;
        m_pixelShaderPath = nullptr;
    }

    if (a_path != nullptr)
    {
        const int len = (int)strlen(a_path) + 1;

        m_pixelShaderPath = new char[len];

        for (int i = 0; i < len; ++i)
        {
            m_pixelShaderPath[i] = a_path[i];
        }
    }

    const char* name = GetShaderProgramName();

    m_shaderProgram = Datastore::LoadShaderProgram(m_vertexShaderPath, m_pixelShaderPath);

    delete[] name;
}

bool ShaderSettings::AddVertexInput(VertexInput* a_value)
{
    for (VertexInput* iter : m_vertexInputs)
    {
        if (iter == a_value)
        {
            return false;
        }
    }

    m_vertexInputs.emplace_back(a_value);

    return true;
}
void ShaderSettings::RemoveVertexInput(VertexInput* a_value)
{
    for (auto iter = m_vertexInputs.begin(); iter != m_vertexInputs.end(); ++iter)
    {
        if (*iter == a_value)
        {
            m_vertexInputs.erase(iter);

            return;
        }
    }
}
void ShaderSettings::PushVertexInput()
{
    VertexInput* input = new VertexInput();
    input->Slot = -1;
    input->Type = VertexInputType_Null;
    input->VertexSlot = VertexInputSlot_Null;

    m_vertexInputs.emplace_back(input);
}
void ShaderSettings::PopVertexInput()
{
    if (m_vertexInputs.size() <= 0)
    {
        return;
    }

    auto iter = --m_vertexInputs.end();
    delete *iter;
    m_vertexInputs.erase(iter);
}
VertexInput* ShaderSettings::GetVertexInput(unsigned int a_index) const
{
    if (a_index < m_vertexInputs.size())
    {
        return m_vertexInputs[a_index];
    }

    return nullptr;
}

bool ShaderSettings::AddShaderVariable(ShaderVariable* a_value)
{
    for (ShaderVariable* iter : m_shaderVars)
    {
        if (iter == a_value)
        {
            return false;
        }
    }

    m_shaderVars.emplace_back(a_value);

    return true;
}
void ShaderSettings::RemoveShaderVariable(ShaderVariable* a_value)
{
    for (auto iter = m_shaderVars.begin(); iter != m_shaderVars.end(); ++iter)
    {
        if (*iter == a_value)
        {
            m_shaderVars.erase(iter);

            return;
        }
    }
}
void ShaderSettings::PushShaderVariable()
{
    ShaderVariable* var = new ShaderVariable();
    var->Slot = -1;
    var->InputType = VariableInputType_Null;
    var->Type = VariableType_Null;
    var->Variable = nullptr;

    m_shaderVars.emplace_back(var);
}
void ShaderSettings::PopShaderVariable()
{
    if (m_shaderVars.size() <= 0)
    {
        return;
    }

    auto iter = --m_shaderVars.end();
    delete *iter;
    m_shaderVars.erase(iter);
}
ShaderVariable* ShaderSettings::GetShaderVariable(unsigned int a_index) const
{
    if (a_index < m_shaderVars.size())
    {
        return m_shaderVars[a_index];
    }

    return nullptr;
}