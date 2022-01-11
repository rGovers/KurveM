#pragma once

#include "Modals/Modal.h"

#include <list>

class Workspace;

class ExportOBJModal : public Modal
{
private:
    Workspace*       m_workspace;
         
    char*            m_path;
    char*            m_name;
         
    char*            m_fPath;

    std::list<char*> m_dirs;
    std::list<char*> m_files;

    bool             m_ret;

    bool             m_smartStep;
    int              m_curveStep;

    int              m_pathStep;
    int              m_shapeStep;

    bool             m_exportSelected;

    void Overwrite(bool a_value);
    
    void Clear();

protected:

public:
    ExportOBJModal(Workspace* a_workspace, const char* a_path);
    ~ExportOBJModal();

    virtual const char* GetName();

    virtual glm::vec2 GetSize();

    virtual bool Execute();
};