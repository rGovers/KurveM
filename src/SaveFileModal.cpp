#include "Modals/SaveFileModal.h"

#include <fstream>
#include <filesystem>
#include <string>
#include <string.h>
#include <vector>

#include "imgui.h"
#include "imgui_internal.h"
#include "Modals/ConfirmModal.h"
#include "Modals/ErrorModal.h"
#include "Workspace.h"

#define PATHSIZE 2048

#if _WIN32
#define BASELENGTH 3
#else
#define BASELENGTH 1
#endif

void SaveFileModal::Overwrite(bool a_value)
{
    if (a_value)
    {
        m_ret = false;

        m_workspace->SaveAs(m_fPath);
    }
}

void SaveFileModal::Clear()
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
void SaveFileModal::GenerateFiles()
{
    if (std::filesystem::exists(m_path))
    {
        std::filesystem::directory_iterator fsIter = std::filesystem::directory_iterator(m_path);

        for (auto iter : fsIter)
        {
            if (iter.is_directory())
            {
                // Have to use string otherwise I get garbage data
                std::string str = iter.path().filename();

                int len = str.length() + 1;

                char* directory = new char[len];

                for (int i = 0; i < len; ++i)
                {
                    directory[i] = str[i];
                }
                directory[len - 1] = 0;

                m_dirs.emplace_back(directory);
            }
            else if (iter.is_regular_file())
            {
                std::string str = iter.path().filename();

                int len = str.length() + 1;

                char* fileName = new char[len];

                for (int i = 0; i < len; ++i)
                {
                    fileName[i] = str[i];
                }
                fileName[len - 1] = 0;

                m_files.emplace_back(fileName);
            }
        }
    }
}

SaveFileModal::SaveFileModal(Workspace* a_workspace, const char* a_path)
{
    m_workspace = a_workspace;

    const int size = strlen(a_path) + 1;

    m_fPath = nullptr;

    m_path = new char[PATHSIZE];

    for (int i = 0; i < size; ++i)
    {
        m_path[i] = a_path[i];
    }

    m_name = new char[PATHSIZE] { 0 };

    m_ret = true;

    GenerateFiles();
}
SaveFileModal::~SaveFileModal()
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

const char* SaveFileModal::GetName()
{
    return "Save File";
}

glm::vec2 SaveFileModal::GetSize()
{
    return glm::vec2(640, 480);
}

bool SaveFileModal::Execute()
{  
    const ImGuiStyle style = ImGui::GetStyle();
    if (ImGui::InputText("Path", m_path, PATHSIZE))
    {
        Clear();
        GenerateFiles();
    }

    ImVec2 region = ImGui::GetContentRegionAvail();

    if (ImGui::BeginChild("Dir", { region.x - style.FramePadding.x, region.y - ImGui::GetFrameHeightWithSpacing() * 2 }))
    {
        if (std::filesystem::exists(m_path))
        {
            ImGui::Columns(3);
            ImGui::BeginGroup();

            if (strlen(m_path) > BASELENGTH)
            {
                if (ImGui::Selectable(".."))
                {
                    int len = strlen(m_path) - 1;

                    int end = -1;

                    for (int i = len; i >= 0; --i)
                    {
                        if (m_path[i] == '/')
                        {
                            end = i;

                            break;
                        }
                    }

                    if (end >= BASELENGTH)
                    {
                        m_path[end] = 0;
                    }
                    else if (end == BASELENGTH - 1)
                    {
#if _WIN32  
                        m_path[1] = ':';
                        m_path[2] = '/';
                        m_path[3] = 0;
#else   
                        m_path[0] = '/';
                        m_path[1] = 0;
#endif      
                    }

                    Clear();
                    GenerateFiles();
                }

                ImGui::NextColumn();
            }

            for (auto iter = m_dirs.begin(); iter != m_dirs.end(); ++iter)
            {
                const char* str = *iter;
                if (ImGui::Selectable(str))
                {
                    int len = strlen(m_path);

                    if (m_path[len - 1] != '/')
                    {
                        m_path[len++] = '/';
                    }

                    const int strL = strlen(str) + 1;

                    for (int i = 0; i < strL; ++i)
                    {
                        m_path[i + len] = str[i];
                    }

                    Clear();
                    GenerateFiles();

                    break;
                }
                ImGui::NextColumn();
            }

            ImGui::EndGroup();
            ImGui::Columns();

            ImGui::Separator();

            ImGui::Columns(3);
            ImGui::BeginGroup();

            for (auto iter = m_files.begin(); iter != m_files.end(); ++iter)
            {
                const char* str = *iter;
                if (ImGui::Selectable(str))
                {
                    const int len = strlen(str) + 1;

                    for (int i = 0; i < len; ++i)
                    {
                        m_name[i] = str[i];
                    }
                }
                ImGui::NextColumn();
            }

            ImGui::EndGroup();
            ImGui::Columns();
        }

        ImGui::EndChild();
    }

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

            if (std::ifstream(m_fPath).good())
            {
                m_workspace->PushModal(new ConfirmModal("Overwrite File", std::bind(&SaveFileModal::Overwrite, this, std::placeholders::_1)));
            }
            else
            {
                m_workspace->SaveAs(m_fPath);

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