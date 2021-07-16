#include "ShaderStorageBuffer.h"

#include <glad/glad.h>

ShaderStorageBuffer::ShaderStorageBuffer(const void* a_data, unsigned int a_size)
{
    glGenBuffers(1, &m_handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, a_size, a_data, GL_STATIC_READ);
}
ShaderStorageBuffer::~ShaderStorageBuffer()
{
    glDeleteBuffers(1, &m_handle);
}

void ShaderStorageBuffer::WriteData(const void* a_data, unsigned int a_size)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, a_size, a_data, GL_STATIC_READ);
}