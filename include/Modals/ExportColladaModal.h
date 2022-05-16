#pragma once

#include "Modals/Modal.h"

#include <list>

class Workspace;

class ExportColladaModal : public Modal
{
private:
    Workspace*       m_workspace;

    char*            m_path;
    char*            m_name;
      
    char*            m_fPath;

    char*            m_author;
    char*            m_copyright;

    bool             m_ret;

    std::list<char*> m_dirs;
    std::list<char*> m_files;

    bool             m_smartStep;
    int              m_curveStep;

    int              m_pathStep;
    int              m_shapeStep;

    bool             m_exportSelected;
    bool             m_exportAnimations;

    void Overwrite(bool a_value);
    
    void Clear();

protected:

public:
    ExportColladaModal(Workspace* a_workspace, const char* a_path);
    ~ExportColladaModal();

    virtual const char* GetName();

    virtual glm::vec2 GetSize();

    virtual bool Execute();
};