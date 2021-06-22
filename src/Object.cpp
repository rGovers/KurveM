#include "Object.h"

#include <glad/glad.h>
#include <string.h>

#include "Camera.h"
#include "CurveModel.h"
#include "Datastore.h"
#include "Model.h"
#include "ShaderPixel.h"
#include "ShaderProgram.h"
#include "Shaders/EditorStandardPixel.h"
#include "Shaders/EditorStandardVertex.h"
#include "ShaderVertex.h"
#include "Transform.h"

long long Object::ObjectIDNum = 0;

Object::Object(const char* a_name)
{
    m_name = nullptr;

    m_transform = new Transform();

    m_parent = nullptr;

    SetName(a_name);

    m_curveModel = nullptr;
    m_displayModel = nullptr;

    m_program = Datastore::GetShaderProgram("SHADER_EDITORSTANDARD");
    if (m_program == nullptr)
    {
        ShaderVertex* vertexShader = new ShaderVertex(EDITORSTANDARDVERTEX);
        ShaderPixel* pixelShader = new ShaderPixel(EDITORSTANDARDPIXEL);

        m_program = new ShaderProgram(vertexShader, pixelShader);

        delete vertexShader;
        delete pixelShader;

        Datastore::AddShaderProgram("SHADER_EDITORSTANDARD", m_program);
    }

    // Not the best but it works
    m_id = ObjectIDNum++;
}
Object::~Object()
{
    if (m_curveModel != nullptr)
    {
        delete m_curveModel;
        m_curveModel = nullptr;
    }

    if (m_displayModel != nullptr)
    {
        delete m_displayModel;
        m_displayModel = nullptr;
    }

    if (m_name != nullptr)
    {
        delete[] m_name;
        m_name = nullptr;
    }

    delete m_transform;

    for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        delete *iter;
    }
}

void Object::SetName(const char* a_name)
{   
    if (m_name != nullptr)
    {
        delete[] m_name;
        m_name = nullptr;
    }

    if (a_name != nullptr)
    {
        int len = strlen(a_name);
        
        if (len != 0)
        {
            m_name = new char[len + 1];

            for (int i = 0; i <= len; ++i)
            {
                m_name[i] = a_name[i];
            }
        }
    }
}

void Object::SetParent(Object* a_parent)
{
    if (m_parent != nullptr)
    {
        for (auto iter = m_parent->m_children.begin(); iter != m_parent->m_children.end(); ++iter)
        {
            if ((*iter)->GetID() == m_id)
            {
                m_parent->m_children.erase(iter);

                break;
            }
        }

        m_parent = nullptr;
    }

    if (a_parent != nullptr)
    {
        m_parent = a_parent;
        m_parent->m_children.emplace_back(this);
    }
}

void Object::Draw(Camera* a_camera, const glm::vec2& a_winSize)
{
    Model* model = m_displayModel;

    if (m_curveModel != nullptr)
    {
        model = m_curveModel->GetDisplayModel();
    }

    if (model != nullptr)
    {
        const unsigned int programHandle = m_program->GetHandle();
        glUseProgram(programHandle);

        const unsigned int vao = model->GetVAO();
        glBindVertexArray(vao);

        const glm::mat4 view = a_camera->GetView();
        const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);

        // const glm::mat4 viewProj = view * proj;
        const glm::mat4 viewProj = proj * view;

        const glm::mat4 world = m_transform->ToMatrix();

        glUniformMatrix4fv(0, 1, false, (float*)&view);
        glUniformMatrix4fv(1, 1, false, (float*)&proj);
        glUniformMatrix4fv(2, 1, false, (float*)&world);

        glDrawElements(GL_TRIANGLES, model->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    }
}