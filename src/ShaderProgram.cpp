#include "ShaderProgram.h"

#include <glad/glad.h>
#include <stdio.h>

#include "Datastore.h"
#include "ShaderPixel.h"
#include "Shaders/AnimatorSoftbodyPixel.h"
#include "Shaders/AnimatorSoftbodySolidPixel.h"
#include "Shaders/AnimatorSoftbodyVertex.h"
#include "Shaders/AnimatorStandardPixel.h"
#include "Shaders/AnimatorStandardSolidPixel.h"
#include "Shaders/AnimatorStandardVertex.h"
#include "Shaders/EditorStandardPixel.h"
#include "Shaders/EditorStandardSolidPixel.h"
#include "Shaders/EditorStandardVertex.h"
#include "Shaders/ReferenceImagePixel.h"
#include "Shaders/ReferenceImageVertex.h"
#include "Shaders/WeightStandardPixel.h"
#include "Shaders/WeightStandardVertex.h"
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
ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_handle);
}

ShaderProgram* ShaderProgram::InitProgram(const char* a_name, const char* a_vertexSource, const char* a_pixelSource)
{
    ShaderVertex* shaderVertex = new ShaderVertex(a_vertexSource);
    ShaderPixel* shaderPixel = new ShaderPixel(a_pixelSource);

    ShaderProgram* program = new ShaderProgram(shaderVertex, shaderPixel);

    delete shaderVertex;
    delete shaderPixel;

    Datastore::AddShaderProgram(a_name, program);

    return program;
}

void ShaderProgram::LoadBaseShaders()
{
    ShaderVertex* vertex = Datastore::GetVertexShader(nullptr);
    ShaderPixel* pixel = Datastore::GetPixelShader(nullptr);

    Datastore::AddShaderProgram("[Base Shader] [Base Shader]", new ShaderProgram(vertex, pixel));
    Datastore::AddShaderProgram(EditorBase, new ShaderProgram(vertex, pixel));

    ShaderProgram::InitProgram(EditorBaseSolid, EDITORSTANDARDVERTEX, EDITORSTANDARDSOLIDPIXEL);
    ShaderProgram::InitProgram(EditorAnimation, ANIMATORSTANDARDVERTEX, ANIMATORSTANDARDPIXEL);
    ShaderProgram::InitProgram(EditorAnimationSolid, ANIMATORSTANDARDVERTEX, ANIMATORSTANDARDSOLIDPIXEL);
    ShaderProgram::InitProgram(EditorAnimationSBody, ANIMATORSOFTBODYVERTEX, ANIMATORSOFTBODYPIXEL);
    ShaderProgram::InitProgram(EditorAnimationSBodySolid, ANIMATORSOFTBODYVERTEX, ANIMATORSOFTBODYSOLIDPIXEL);
    ShaderProgram::InitProgram(EditorWeight, WEIGHTSTANDARDVERTEX, WEIGHTSTANDARDPIXEL);
    ShaderProgram::InitProgram(EditorReferenceImage, REFERENCEIMAGEVERTEX, REFERENCEIMAGEPIXEL);
}