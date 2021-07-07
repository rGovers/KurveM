#include "Modals/Modal.h"

#include "imgui.h"

bool Modal::Open()
{
    bool ret = true;

    const char* name = GetName();

    const glm::vec2 size = GetSize();
    if (size.x > 0 && size.y > 0)
    {
        ImGui::SetNextWindowSize({ size.x, size.y });
    }
    if (ImGui::BeginPopupModal(name, nullptr, ImGuiWindowFlags_NoResize))
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