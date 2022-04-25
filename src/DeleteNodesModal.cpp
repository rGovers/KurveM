#include "Modals/DeleteNodesModal.h"

#include <stdio.h>

#include "Actions/DeleteCurveNodeAction.h"
#include "Actions/DeletePathNodeAction.h"
#include "EditorControls/Editor.h"
#include "imgui.h"
#include "Workspace.h"

DeleteNodesModal::DeleteNodesModal(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_indices, unsigned int a_indexCount, CurveModel* a_curveModel)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_curveModel = a_curveModel;
    m_pathModel = nullptr;

    m_indexCount = a_indexCount;

    m_indices = new unsigned int[m_indexCount];
    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        m_indices[i] = a_indices[i];
    }
}
DeleteNodesModal::DeleteNodesModal(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_indices, unsigned int a_indexCount, PathModel* a_model)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_curveModel = nullptr;
    m_pathModel = a_model;

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
    return "Delete";
}

bool DeleteNodesModal::Execute() 
{    
    if (ImGui::Button("Delete Nodes"))
    {
        Action* action = nullptr;
        if (m_curveModel != nullptr)
        {
            action = new DeleteCurveNodeAction(m_workspace, m_editor, m_indices, m_indexCount, m_curveModel);
        }
        else if (m_pathModel != nullptr)
        {
            action = new DeletePathNodeAction(m_workspace, m_editor, m_indices, m_indexCount, m_pathModel);
        }

        if (action != nullptr)
        {
            if (!m_workspace->PushAction(action))
            {
                printf("Error Deleting Node \n");

                delete action;
            }
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