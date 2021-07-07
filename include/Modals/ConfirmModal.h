#pragma once

#include "Modals/Modal.h"

#include <functional>

class ConfirmModal : public Modal
{
private:
    char*                 m_msg;
    
    // Writing function pointers by hand is the worst
    // The one thing I continue to refuse to do
    std::function<void(bool)> m_callback;

protected:

public:
    ConfirmModal(const char* a_msg, const std::function<void(bool)>& a_callback);
    ~ConfirmModal();

    virtual const char* GetName();

    virtual glm::vec2 GetSize();

    virtual bool Execute();
};