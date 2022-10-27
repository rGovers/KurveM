#pragma once

#include <vector>

class ShaderPixel;
class ShaderProgram;
class ShaderVertex;

struct ShaderVariable;
struct VertexInput;

class ShaderSettings
{
private:
    char*                        m_vertexShaderPath;
    char*                        m_pixelShaderPath;
      
    ShaderProgram*               m_shaderProgram;

    std::vector<VertexInput*>    m_vertexInputs;
    std::vector<ShaderVariable*> m_shaderVars;

    char* GetShaderProgramName() const;

protected:

public:
    ShaderSettings();
    ~ShaderSettings();

    inline const char* GetVertexShaderPath() const
    {
        return m_vertexShaderPath;
    }
    inline const char* GetPixelShaderPath() const
    {
        return m_pixelShaderPath;
    }

    void SetVertexShaderPath(const char* a_path);
    void SetPixelShaderPath(const char* a_path);

    bool AddVertexInput(VertexInput* a_value);
    void RemoveVertexInput(VertexInput* a_value);
    void PushVertexInput();
    void PopVertexInput();
    VertexInput* GetVertexInput(unsigned int a_index) const;
    inline unsigned int GetVertexInputSize() const
    {
        return (unsigned int)m_vertexInputs.size();
    }

    bool AddShaderVariable(ShaderVariable* a_value);
    void RemoveShaderVariable(ShaderVariable* a_value);
    void PushShaderVariable();
    void PopShaderVariable();
    ShaderVariable* GetShaderVariable(unsigned int a_index) const;
    inline unsigned int GetShaderVariableSize() const
    {
        return (unsigned int)m_shaderVars.size();
    }

    inline ShaderProgram* GetShader() const
    {
        return m_shaderProgram;
    }
};