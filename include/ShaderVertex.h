#pragma once

class ShaderVertex
{
private:
    unsigned int m_handle;
protected:

public:
    ShaderVertex(const char* a_source);
    ~ShaderVertex();

    inline unsigned int GetHandle() const
    {
        return m_handle;
    }
};