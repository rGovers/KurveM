#include "ShaderProgram.h"

#include <glad/glad.h>
#include <stdio.h>

#include "ShaderPixel.h"
#include "ShaderVertex.h"

ShaderProgram::ShaderProgram(ShaderVertex* a_vertexShader, ShaderPixel* a_pixelShader)
{
    m_handle = glCreateProgram();

    glAttachShader(m_handle, a_vertexShader->GetHandle());
    glAttachShader(m_handle, a_pixelShader->GetHandle());

    glLinkProgram(m_handle);

    int success;
    glGetProgramiv(m_handle, GL_LINK_STATUS, (int *)&success);
    if (success == GL_FALSE)
    {
        int len;
        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &len);

        char* buff = new char[len];
        glGetProgramInfoLog(m_handle, len, nullptr, buff);

        printf(buff);
        printf("\n");

        delete[] buff;
    }

    glDetachShader(m_handle, a_vertexShader->GetHandle());
    glDetachShader(m_handle, a_pixelShader->GetHandle());
}