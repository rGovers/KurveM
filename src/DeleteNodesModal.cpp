#include "Modals/DeleteNodesModal.h"

#include <stdio.h>

#include "Actions/DeleteNodeAction.h"
#include "Editor.h"
#include "imgui.h"
#include "Workspace.h"

DeleteNodesModal::DeleteNodesModal(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_indices, unsigned int a_indexCount, CurveModel* a_curveModel)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_model = a_curveModel;

    m_indexCount = a_indexCount;

    m_indices = new unsigned int[m_indexCount];
    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        m_indices[i] = a_indices[i];
    }
}
DeleteNodesModal::~DeleteNodesModal()
{
    delete[] m_indices;
}

const char* DeleteNodesModal::GetName()
{
    return "Delete Nodes";
}

bool DeleteNodesModal::Execute() 
{    
    if (ImGui::Button("Delete Nodes"))
    {
        Action* action = new DeleteNodeAction(m_workspace, m_editor, m_indices, m_indexCount, m_model);

        if (!m_workspace->PushAction(action))
        {
            printf("Error Deleting Node \n");

            delete action;
        }

        return false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
    {
        return false;
    }

    return true;
}