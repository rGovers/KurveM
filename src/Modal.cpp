#include "Modals/Modal.h"

#include "imgui.h"

bool Modal::Open()
{
    bool ret = true;

    const char* name = GetName();

    if (ImGui::BeginPopupModal(name))
    {
        ret = Execute();
        
        ImGui::EndPopup();
    }

    // Needs to be called after otherwise the modal never pops up
    ImGui::OpenPopup(name);

    if (ret == false)
    {
        ImGui::CloseCurrentPopup();
    }

    return ret;
}