#include "Modals/LoadShaderModal.h"

#include <fstream>
#include <string.h>

#include "Actions/SetPixelShaderAction.h"
#include "Actions/SetVertexShaderAction.h"
#include "Datastore.h"
#include "FileDialog.h"
#include "imgui.h"
#include "Modals/ErrorModal.h"
#include "Workspace.h"

void LoadShaderModal::Clear()
{
    for (auto iter = m_dirs.begin(); iter != m_dirs.end(); ++iter)
    {
        delete[] *iter;
    }
    m_dirs.clear();

    for (auto iter = m_files.begin(); iter != m_files.end(); ++iter)
    {
        delete[] *iter;
    }
    m_files.clear();
}

LoadShaderModal::LoadShaderModal(Workspace* a_workspace, const char* a_path, Object* const* a_objects, unsigned int a_objectCount, e_ShaderLoadMode a_mode)
{
    m_mode = a_mode;

    m_workspace = a_workspace;

    const int size = (int)strlen(a_path) + 1;

    m_fPath = nullptr;

    m_path = new char[PathSize];

    for (int i = 0; i < size; ++i)
    {
        if (a_path[i] == '\\')
        {
            m_path[i] = '/';

            continue;
        }

        m_path[i] = a_path[i];
    }

    m_objectCount = a_objectCount;
    m_objects = new Object*[m_objectCount];

    for (int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i] = a_objects[i];
    }

    m_name = new char[PathSize] { 0 };

    GenerateDirectoryData();
}
LoadShaderModal::~LoadShaderModal()
{
    delete[] m_path;
    delete[] m_name;

    if (m_fPath != nullptr)
    {
        delete[] m_fPath;
        m_fPath = nullptr;
    }

    delete[] m_objects;

    Clear();
}

void LoadShaderModal::GenerateDirectoryData()
{
    switch (m_mode)
    {
    case ShaderLoadMode_Vertex:
    {
        FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, VertexFileExts, VertexExtCount);

        break;
    }
    case ShaderLoadMode_Pixel:
    {
        FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, PixelFileExts, PixelExtCount);

        break;
    }
    }
}

const char* LoadShaderModal::GetName()
{
    switch (m_mode)
    {
    case ShaderLoadMode_Vertex:
    {
        return "Load Vertex Shader";

        break;
    }
    case ShaderLoadMode_Pixel:
    {
        return "Load Pixel Shader";

        break;
    }
    }

    return "LoadShader";
}

glm::vec2 LoadShaderModal::GetSize()
{
    return glm::vec2(640.0f, 480.0f);
}

bool LoadShaderModal::Execute()
{
    if (ImGui::InputText("Path", m_path, PathSize))
    {
        Clear();
        GenerateDirectoryData();
    }

    if (!FileDialog::PartialExplorer(m_dirs, m_files, m_path, m_name))
    {
        Clear();
        GenerateDirectoryData();
    }

    ImGui::SameLine();

    ImGui::BeginGroup();

    ImGui::EndGroup();

    ImGui::InputText("Name", m_name, PathSize);

    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        if (m_name[0] == 0)
        {
            m_workspace->PushModal(new ErrorModal("Invalid file name"));
        }
        else
        {
            int pathLen = (int)strlen(m_path);

            if (m_path[pathLen - 1] != '/')
            {
                m_path[pathLen++] = '/';
                m_path[pathLen] = 0;
            }

            const char* namePtr = strchr(m_name, '.');
            const int nameLen = (int)strlen(m_name);

            if (namePtr == nullptr)
            {
                if (m_fPath != nullptr)
                {
                    delete[] m_fPath;
                    m_fPath = nullptr;
                }

                const int namePathSize = pathLen + nameLen;
                m_fPath = new char[PathSize];

                for (int j = 0; j < pathLen; ++j)
                {
                    m_fPath[j] = m_path[j];
                }
                for (int j = 0; j < nameLen; ++j)
                {
                    m_fPath[j + pathLen] = m_name[j];
                }

                switch (m_mode)
                {
                case ShaderLoadMode_Vertex:
                {
                    for (int i = 0; i < VertexExtCount; ++i)
                    {
                        const char* ext = VertexFileExts[i];
                        const int extSize = (int)strlen(ext) + 1;

                        for (int j = 0; j < extSize; ++j)
                        {
                            m_fPath[j + namePathSize] = ext[j];
                        }

                        std::ifstream file = std::ifstream(m_fPath);
                        if (file.good())
                        {
                            file.close();

                            ShaderVertex* shader = Datastore::GetVertexShader(m_fPath);
                            if (shader != nullptr)
                            {
                                goto VertexEndLoop;
                            }

                            break;
                        }
                    }

                    m_workspace->PushModal(new ErrorModal("Cannot load vertex file"));

VertexEndLoop:;

                    break;
                }
                case ShaderLoadMode_Pixel:
                {
                    for (int i = 0; i < PixelExtCount; ++i)
                    {
                        const char* ext = PixelFileExts[i];
                        const int extSize = (int)strlen(ext) + 1;

                        for (int j = 0; j < extSize; ++j)
                        {
                            m_fPath[j + namePathSize] = ext[j];
                        }

                        std::ifstream file = std::ifstream(m_fPath);
                        if (file.good())
                        {
                            file.close();

                            ShaderPixel* shader = Datastore::GetPixelShader(m_fPath);
                            if (shader != nullptr)
                            {
                                goto PixelEndLoop;
                            }

                            break;
                        }
                    }

                    m_workspace->PushModal(new ErrorModal("Cannot load pixel file"));

PixelEndLoop:;

                    break;
                }
                }
            }
            else
            {
                const int size = pathLen + nameLen + 1;

                if (m_fPath != nullptr)
                {
                    delete[] m_fPath;
                    m_fPath = nullptr;
                }

                m_fPath = new char[size];
                for (int i = 0; i < pathLen; ++i)
                {
                    m_fPath[i] = m_path[i];
                }

                for (int i = 0; i <= nameLen; ++i)
                {
                    m_fPath[i + pathLen] = m_name[i];
                }

                switch (m_mode)
                {
                case ShaderLoadMode_Vertex:
                {
                    ShaderVertex* shader = Datastore::GetVertexShader(m_fPath);
                    if (shader != nullptr)
                    {
                        m_workspace->PushActionSet(new SetVertexShaderAction(m_workspace, m_objects, m_objectCount, m_fPath), "Cannot set vertex shader");

                        return false;
                    }
                    else
                    {
                        m_workspace->PushModal(new ErrorModal("Unable to select vertex shader"));
                    }

                    break;
                }
                case ShaderLoadMode_Pixel:
                {
                    ShaderPixel* shader = Datastore::GetPixelShader(m_fPath);
                    if (shader != nullptr)
                    {
                        m_workspace->PushActionSet(new SetPixelShaderAction(m_workspace, m_objects, m_objectCount, m_fPath), "Cannot set pixel shader");

                        return false;
                    }
                    else
                    {
                        m_workspace->PushModal(new ErrorModal("Unable to select pixel shader"));
                    }

                    break;
                }
                }
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
    {
        return false;
    }

    return true;
}