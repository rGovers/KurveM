#pragma once

class ShaderVertex;
class ShaderPixel;

class ShaderProgram
{
private:
    unsigned int m_handle;

protected:

public:
    ShaderProgram(ShaderVertex* a_vertexShader, ShaderPixel* a_pixelShader);
    ~ShaderProgram();

    inline unsigned int GetHandle() const
    {
        return m_handle;
    }

    static ShaderProgram* InitProgram(const char* a_name, const char* a_vertexSource, const char* a_pixelSource);
};