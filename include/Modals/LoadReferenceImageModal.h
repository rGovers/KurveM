#pragma once

#include "Modals/Modal.h"

#include <list>

class Object;
class Workspace;

class LoadReferenceImageModal : public Modal
{
private:
    Workspace*       m_workspace;
         
    Object*          m_parent;

    char*            m_path;
    char*            m_name;
         
    char*            m_fPath;

    std::list<char*> m_dirs;
    std::list<char*> m_files;

    char*            m_ext;

    void Overwrite(bool a_value);
    
    void Clear();

protected:

public:
    LoadReferenceImageModal(Workspace* a_workspace, Object* a_parent, const char* a_path, const char* a_ext);
    ~LoadReferenceImageModal();

    virtual const char* GetName();

    virtual glm::vec2 GetSize();

    virtual bool Execute();
};