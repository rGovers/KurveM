#include "Modals/CreateAnimationModal.h"

#include <stdio.h>

#include "Actions/CreateAnimationAction.h"
#include "imgui.h"
#include "Modals/ErrorModal.h"
#include "Workspace.h"

#define BUFFER_SIZE 2048

CreateAnimationModal::CreateAnimationModal(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_name = new char[BUFFER_SIZE] { 0 };
}
CreateAnimationModal::~CreateAnimationModal()
{
    delete[] m_name;
}

const char* CreateAnimationModal::GetName()
{
    return "Create Animation";
}

glm::vec2 CreateAnimationModal::GetSize()
{
    const float spacing = ImGui::GetFrameHeightWithSpacing();

    return glm::vec2(256, spacing * 3.5f);
}

bool CreateAnimationModal::Execute()
{
    ImGui::InputText("Name", m_name, BUFFER_SIZE);

    if (ImGui::Button("OK"))
    {
        if (m_name[0] != 0)
        {
            Action* action = new CreateAnimationAction(m_workspace, m_name);
            if (!m_workspace->PushAction(action))
            {
                printf("Failed to create animation \n");

                delete action;
            }

            return false;
        }
        else
        {
            m_workspace->PushModal(new ErrorModal("Invalid Name"));
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
    {
        return false;
    }

    return true;
}