#include "FileDialog.h"

#include <filesystem>

#include "imgui.h"

#if _WIN32
#define BASELENGTH 3
#else
#define BASELENGTH 1
#endif

bool FileDialog::GenerateFilesAndDirs(std::list<char*>* a_dirs, std::list<char*>* a_files, const char* a_path, const char* a_ext)
{
    if (std::filesystem::exists(a_path))
    {
        // So apparently on windows there seems to be a weird bug that if you have the permission for a directory it is not letting me in until I set the permission denied flag
        std::filesystem::directory_iterator fsIter = std::filesystem::directory_iterator(a_path, std::filesystem::directory_options::skip_permission_denied);

        for (auto iter : fsIter)
        {
            if (iter.is_directory())
            {
                // Have to use string otherwise I get garbage data
                const std::string str = iter.path().filename().u8string();

                const int len = str.length() + 1;

                char* directory = new char[len];

                for (int i = 0; i < len; ++i)
                {
                    directory[i] = str[i];
                }
                directory[len - 1] = 0;

                a_dirs->emplace_back(directory);
            }
            else if (iter.is_regular_file())
            {
                const std::string extStr = iter.path().extension().u8string();
                if (extStr == a_ext)
                {
                    const std::string str = iter.path().filename().u8string();

                    const int len = str.length() + 1;

                    char* fileName = new char[len];

                    for (int i = 0; i < len; ++i)
                    {
                        fileName[i] = str[i];
                    }
                    fileName[len - 1] = 0;

                    a_files->emplace_back(fileName);
                }
            }
        }

        return true;
    }

    return false;
}

bool FileDialog::DirectoryExplorer(const std::list<char*>& a_dirs, char* a_path)
{
    if (strlen(a_path) > BASELENGTH)
    {
        if (ImGui::Selectable(".."))
        {
            const int len = strlen(a_path) - 1;

            int end = -1;

            for (int i = len; i >= 0; --i)
            {
                if (a_path[i] == '/' || a_path[i] == '\\')
                {
                    end = i;
    
                    break;
                }
            }

            if (end >= BASELENGTH)
            {
                a_path[end] = 0;
            }
            else if (end == BASELENGTH - 1)
            {
#if _WIN32  
                a_path[1] = ':';
                a_path[2] = '/';
                a_path[3] = 0;
#else   
                a_path[0] = '/';
                a_path[1] = 0;
#endif      
            }

            return false;
        }

        ImGui::NextColumn();
    }

    for (auto iter = a_dirs.begin(); iter != a_dirs.end(); ++iter)
    {
        const char* str = *iter;
        if (ImGui::Selectable(str))
        {
            int len = strlen(a_path);

            if (a_path[len - 1] != '/')
            {
                a_path[len++] = '/';
            }

            const int strL = strlen(str) + 1;

            for (int i = 0; i < strL; ++i)
            {
                a_path[i + len] = str[i];
            }

            return false;
        }

        ImGui::NextColumn();
    }

    return true;
}
void FileDialog::FileExplorer(const std::list<char*>& a_files, char* a_name)
{
    for (auto iter = a_files.begin(); iter != a_files.end(); ++iter)
    {
        const char* str = *iter;
        if (ImGui::Selectable(str))
        {
            const int len = strlen(str) + 1;

            for (int i = 0; i < len; ++i)
            {
                a_name[i] = str[i];
            }
        }

        ImGui::NextColumn();
    }
}

bool FileDialog::PartialExplorer(const std::list<char*>& a_dirs, const std::list<char*>& a_files, char* a_path, char* a_name)
{
    bool ret = true;

    const ImGuiStyle style = ImGui::GetStyle();
    const ImVec2 region = ImGui::GetContentRegionAvail();

    if (ImGui::BeginChild("Dir", { region.x - style.FramePadding.x - 300.0f, region.y - ImGui::GetFrameHeightWithSpacing() * 2 }))
    {
        if (std::filesystem::exists(a_path))
        {
            ImGui::Columns(2);
            ImGui::BeginGroup();

            if (!DirectoryExplorer(a_dirs, a_path))
            {
                ret = false;
            }

            ImGui::EndGroup();
            ImGui::Columns();

            ImGui::Separator();

            ImGui::Columns(2);
            ImGui::BeginGroup();

            FileExplorer(a_files, a_name);

            ImGui::EndGroup();
            ImGui::Columns();
        }

        ImGui::EndChild();
    }

    return ret;
}
bool FileDialog::FullExplorer(const std::list<char*>& a_dirs, const std::list<char*>& a_files, char* a_path, char* a_name)
{
    bool ret = true;

    const ImGuiStyle style = ImGui::GetStyle();
    const ImVec2 region = ImGui::GetContentRegionAvail();

    if (ImGui::BeginChild("Dir", { region.x - style.FramePadding.x, region.y - ImGui::GetFrameHeightWithSpacing() * 2 }))
    {
        if (std::filesystem::exists(a_path))
        {
            ImGui::Columns(3);
            ImGui::BeginGroup();

            if (!DirectoryExplorer(a_dirs, a_path))
            {
                ret = false;
            }

            ImGui::EndGroup();
            ImGui::Columns();

            ImGui::Separator();

            ImGui::Columns(3);
            ImGui::BeginGroup();

            FileExplorer(a_files, a_name);

            ImGui::EndGroup();
            ImGui::Columns();
        }

        ImGui::EndChild();
    }

    return ret;
}