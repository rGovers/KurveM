#pragma once

class ShaderStorageBuffer
{
private:
    unsigned int m_handle;

protected:

public:
    ShaderStorageBuffer(const void* a_data, unsigned int a_size);
    ~ShaderStorageBuffer();

    inline unsigned int GetHandle() const
    {
        return m_handle;
    }

    void WriteData(const void* a_data, unsigned int a_size);
};