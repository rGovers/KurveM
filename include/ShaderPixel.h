#pragma once

class ShaderPixel
{
private:
    unsigned int m_handle;
protected:

public:
    ShaderPixel(const char* a_source);
    ~ShaderPixel();

    inline unsigned int GetHandle() const
    {
        return m_handle;
    }
};