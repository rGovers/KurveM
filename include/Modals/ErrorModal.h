#pragma once

#include "Modals/Modal.h"

class ErrorModal : public Modal
{
private:
    char* m_msg;
    
protected:

public:
    ErrorModal(const char* a_msg);
    ~ErrorModal();

    virtual const char* GetName();

    virtual glm::vec2 GetSize();

    virtual bool Execute();
};