#include "Modals/ConfirmModal.h"

#include <string.h>

#include "imgui.h"

ConfirmModal::ConfirmModal(const char* a_msg, const std::function<void(bool)>& a_callback)
{
    m_callback = a_callback;

    const int len = strlen(a_msg) + 1;

    m_msg = new char[len];

    for (int i = 0; i < len; ++i)
    {
        m_msg[i] = a_msg[i];
    }
}
ConfirmModal::~ConfirmModal()
{
    delete[] m_msg;
}

const char* ConfirmModal::GetName()
{
    return "Confirm";
}

glm::vec2 ConfirmModal::GetSize()
{
    const float spacing = ImGui::GetFrameHeightWithSpacing();

    return glm::vec2(ImGui::CalcTextSize(m_msg).x + spacing, spacing * 3.25f);
}

bool ConfirmModal::Execute()
{
    ImGui::Text(m_msg);

    if (ImGui::Button("OK"))
    {
        m_callback(true);

        return false;
    }

    ImGui::SameLine();
    
    if (ImGui::Button("Cancel"))
    {
        m_callback(false);

        return false;
    }

    return true;
}