#pragma once

#include "Modals/Modal.h"

class CurveModel;
class Editor;
class Workspace;

class DeleteNodesModal : public Modal
{
private:
    Workspace*    m_workspace;
    Editor*       m_editor;

    CurveModel*   m_model;

    unsigned int  m_indexCount;
    unsigned int* m_indices;

protected:

public:
    DeleteNodesModal(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_indices, unsigned int a_indexCount, CurveModel* a_model);
    ~DeleteNodesModal();

    virtual const char* GetName();

    virtual bool Execute();
};