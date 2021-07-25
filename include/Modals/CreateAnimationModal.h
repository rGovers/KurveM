#pragma once

#include "Modals/Modal.h"

class Workspace;

class CreateAnimationModal : public Modal
{
private:
    Workspace* m_workspace;

    char*      m_name;

protected:

public:
    CreateAnimationModal(Workspace* a_workspace);
    ~CreateAnimationModal();

    virtual const char* GetName();
    
    virtual glm::vec2 GetSize();

    virtual bool Execute();
};