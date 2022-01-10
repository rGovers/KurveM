#include "Modals/ExportOBJModal.h"

#include "FileDialog.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Modals/ConfirmModal.h"
#include "Modals/ErrorModal.h"
#include "Workspace.h"

#define PATHSIZE 2048

void ExportOBJModal::Overwrite(bool a_value)
{
    if (a_value)
    {
        m_ret = false;

        m_workspace->ExportOBJ(m_fPath, m_exportSelected, m_smartStep, m_step);
    }
}

void ExportOBJModal::Clear()
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

ExportOBJModal::ExportOBJModal(Workspace* a_workspace, const char* a_path)
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

    m_exportSelected = false;
    m_smartStep = true;
    m_step = 10;

    m_ret = true;

    FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, ".obj");
}
ExportOBJModal::~ExportOBJModal()
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

const char* ExportOBJModal::GetName()
{
    return "Export Wavefront OBJ";
}

glm::vec2 ExportOBJModal::GetSize()
{
    return glm::vec2(1280, 720);
}

bool ExportOBJModal::Execute()
{  
    if (ImGui::InputText("Path", m_path, PATHSIZE))
    {
        Clear();
        FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, ".obj");
    }

    if (!FileDialog::PartialExplorer(m_dirs, m_files, m_path, m_name))
    {
        Clear();
        FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, ".obj");
    }

    ImGui::SameLine();

    ImGui::BeginGroup();

    ImGui::PushItemWidth(150.0f);
    
    ImGui::Checkbox("Export Selected Objects", &m_exportSelected);

    ImGui::Separator();

    ImGui::Checkbox("Smart Step", &m_smartStep);

    if (ImGui::InputInt("Resolution", &m_step))
    {
        m_step = glm::max(m_step, 1);
    }

    ImGui::PopItemWidth();

    ImGui::EndGroup();

    ImGui::InputText("Name", m_name, PATHSIZE);

    ImGui::SameLine();

    if (ImGui::Button("Save"))
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
                const char* ext = ".obj";

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

            if (std::ifstream(m_fPath).good())
            {
                m_workspace->PushModal(new ConfirmModal("Overwrite File", std::bind(&ExportOBJModal::Overwrite, this, std::placeholders::_1)));
            }
            else
            {
                m_workspace->ExportOBJ(m_fPath, m_exportSelected, m_smartStep, m_step);

                return false;
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
    {
        return false;
    }

    return m_ret;
}