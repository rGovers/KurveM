#pragma once

#include <list>

class FileDialog
{
private:
    static bool DirectoryExplorer(const std::list<char*>& a_dirs, char* a_path);
    static void FileExplorer(const std::list<char*>& a_files, char* a_name);

protected:

public:
    static bool GenerateFilesAndDirs(std::list<char*>* a_dirs, std::list<char*>* a_files, const char* a_path, const char* a_ext);
    static bool GenerateFilesAndDirs(std::list<char*>* a_dirs, std::list<char*>* a_files, const char* a_path, const char* const* a_ext, int a_extCount);

    static bool PartialExplorer(const std::list<char*>& a_dirs, const std::list<char*>& a_files, char* a_path, char* a_name);
    static bool FullExplorer(const std::list<char*>& a_dirs, const std::list<char*>& a_files, char* a_path, char* a_name);
};