#include "Modals/LoadFileModal.h"

#include "FileDialog.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Modals/ConfirmModal.h"
#include "Modals/ErrorModal.h"
#include "Workspace.h"

#define PATHSIZE 2048

void LoadFileModal::Clear()
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

LoadFileModal::LoadFileModal(Workspace* a_workspace, const char* a_path)
{
    m_workspace = a_workspace;

    const int size = strlen(a_path) + 1;

    m_fPath = nullptr;

    m_path = new char[PATHSIZE];

    for (int i = 0; i < size; ++i)
    {
        if (a_path[i] == '\\')
        {
            m_path[i] = '/';

            continue;
        }

        m_path[i] = a_path[i];
    }

    m_name = new char[PATHSIZE] { 0 };

    FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, ".kumSC");
}
LoadFileModal::~LoadFileModal()
{
    delete[] m_path;
    delete[] m_name;

    if (m_fPath != nullptr)
    {
        delete[] m_fPath;
        m_fPath = nullptr;
    }

    Clear();
}

const char* LoadFileModal::GetName()
{
    return "Load File";
}

glm::vec2 LoadFileModal::GetSize()
{
    return glm::vec2(640, 480);
}

bool LoadFileModal::Execute()
{  
    if (ImGui::InputText("Path", m_path, PATHSIZE))
    {
        Clear();
        FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, ".kumSC");
    }

    if (!FileDialog::FullExplorer(m_dirs, m_files, m_path, m_name))
    {
        Clear();
        FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, ".kumSC");
    }

    ImGui::InputText("Name", m_name, PATHSIZE);

    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        if (m_name[0] == 0)
        {
            m_workspace->PushModal(new ErrorModal("Invalid file name"));
        }
        else
        {
            int pathLen = strlen(m_path);

            if (m_path[pathLen - 1] != '/')
            {
                m_path[pathLen++] = '/';
                m_path[pathLen] = 0;
            }

            char* namePtr = strchr(m_name, '.');

            if (namePtr == nullptr)
            {
                const char* ext = ".kumSC";

                int nameLen = strlen(m_name);
                int extLen = strlen(ext) + 1;

                for (int i = 0; i < extLen; ++i)
                {
                    m_name[i + nameLen] = ext[i];
                }
            }

            int nameLen = strlen(m_name);

            int size = pathLen + nameLen + 1;

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

            for (int i = 0; i < nameLen + 1; ++i)
            {
                m_fPath[i + pathLen] = m_name[i];
            }

            if (!std::ifstream(m_fPath).good())
            {
                m_workspace->PushModal(new ErrorModal("No file found"));
            }
            else
            {
                m_workspace->Open(m_fPath);

                return false;
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