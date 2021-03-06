#include "Modals/ExportColladaModal.h"

#include <fstream>
#include <string.h>

#include "FileDialog.h"
#include "imgui.h"
#include "Modals/ConfirmModal.h"
#include "Modals/ErrorModal.h"
#include "Workspace.h"

#define PATHSIZE 2048

void ExportColladaModal::Overwrite(bool a_value)
{
    if (a_value)
    {
        m_ret = false;

        m_workspace->ExportCollada(m_fPath, m_exportAnimations, m_exportSelected, m_smartStep, m_curveStep, m_pathStep, m_shapeStep, m_author, m_copyright);
    }
}

void ExportColladaModal::Clear()
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

ExportColladaModal::ExportColladaModal(Workspace* a_workspace, const char* a_path)
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

    m_author = new char[PATHSIZE] { 0 };
    m_copyright = new char[PATHSIZE] { 0 };

    m_exportSelected = false;

    m_exportAnimations = true;

    m_ret = true;

    m_curveStep = 10;
    m_smartStep = false;

    m_pathStep = 5;
    m_shapeStep = 2;

    FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, ".dae");
}
ExportColladaModal::~ExportColladaModal()
{
    Clear();

    if (m_fPath != nullptr)
    {
        delete[] m_fPath;
        m_fPath = nullptr;
    }

    delete[] m_name;
    delete[] m_path;

    delete[] m_author;
    delete[] m_copyright;
}

const char* ExportColladaModal::GetName()
{
    return "Export Collada DAE";
}

glm::vec2 ExportColladaModal::GetSize()
{
    return glm::vec2(1280, 720);
}

bool ExportColladaModal::Execute()
{
    if (ImGui::InputText("Path", m_path, PATHSIZE))
    {
        Clear();
        FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, ".dae");
    }

    if (!FileDialog::PartialExplorer(m_dirs, m_files, m_path, m_name))
    {
        Clear();
        FileDialog::GenerateFilesAndDirs(&m_dirs, &m_files, m_path, ".dae");
    }

    ImGui::SameLine();

    ImGui::BeginGroup();

    ImGui::PushItemWidth(150.0f);

    ImGui::InputText("Author", m_author, PATHSIZE);
    ImGui::InputText("Copyright", m_copyright, PATHSIZE);

    ImGui::Separator();

    ImGui::Checkbox("Export Selected Objects", &m_exportSelected);

    ImGui::Separator();

    ImGui::Text("Curve Model Settings");

    ImGui::Checkbox("Smart Step", &m_smartStep);

    if (ImGui::InputInt("Curve Resolution", &m_curveStep))
    {
        m_curveStep = glm::max(m_curveStep, 1);
    }

    ImGui::Separator();

    ImGui::Text("Path Model Settings");

    if (ImGui::InputInt("Path Resolution", &m_pathStep))
    {
        m_pathStep = glm::max(m_pathStep, 1);
    }
    if (ImGui::InputInt("Shape Resolution", &m_shapeStep))
    {
        m_shapeStep = glm::max(m_shapeStep, 1);
    }

    ImGui::Separator();

    ImGui::Checkbox("Export Animations", &m_exportAnimations);

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
                const char* ext = ".dae";

                const int nameLen = strlen(m_name);
                const int extLen = strlen(ext) + 1;

                for (int i = 0; i < extLen; ++i)
                {
                    m_name[i + nameLen] = ext[i];
                }
            }

            const int nameLen = strlen(m_name);

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

            for (int i = 0; i < nameLen + 1; ++i)
            {
                m_fPath[i + pathLen] = m_name[i];
            }

            if (std::ifstream(m_fPath).good())
            {
                m_workspace->PushModal(new ConfirmModal("Overwrite File", std::bind(&ExportColladaModal::Overwrite, this, std::placeholders::_1)));
            }
            else
            {
                m_workspace->ExportCollada(m_fPath, m_exportAnimations, m_exportSelected, m_smartStep, m_curveStep, m_pathStep, m_shapeStep, m_author, m_copyright);

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