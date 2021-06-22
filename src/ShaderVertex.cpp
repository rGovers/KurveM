#include "ShaderVertex.h"

#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

ShaderVertex::ShaderVertex(const char* a_source)
{
    m_handle = glCreateShader(GL_VERTEX_SHADER);

    int len = strlen(a_source);

    glShaderSource(m_handle, 1, &a_source, &len);
    glCompileShader(m_handle);

    GLint success = 0;
    glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint logSize = 0;
        glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &logSize);

        char* buffer = new char[logSize];
        glGetShaderInfoLog(m_handle, logSize, nullptr, buffer);

        printf(buffer);
        printf("\n");

        delete[] buffer;
    }
}
ShaderVertex::~ShaderVertex()
{
    glDeleteShader(m_handle);
}