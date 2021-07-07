#pragma once

#include "Modals/Modal.h"

#include <list>

class Workspace;

class SaveFileModal : public Modal
{
private:
    Workspace*       m_workspace;
         
    char*            m_path;
    char*            m_name;
         
    char*            m_fPath;

    std::list<char*> m_dirs;
    std::list<char*> m_files;

    bool             m_ret;

    void Overwrite(bool a_value);
    
    void Clear();
    void GenerateFiles();

protected:

public:
    SaveFileModal(Workspace* a_workspace, const char* a_path);
    ~SaveFileModal();

    virtual const char* GetName();

    virtual glm::vec2 GetSize();

    virtual bool Execute();
};