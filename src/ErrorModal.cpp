#include "Modals/ErrorModal.h"

#include <string.h>

#include "imgui.h"

ErrorModal::ErrorModal(const char* a_msg)
{
    const int len = strlen(a_msg) + 1;

    m_msg = new char[len];

    for (int i = 0; i < len; ++i)
    {
        m_msg[i] = a_msg[i];
    }
}
ErrorModal::~ErrorModal()
{
    delete[] m_msg;
}

const char* ErrorModal::GetName()
{
    return "Error";
}

glm::vec2 ErrorModal::GetSize()
{
    const float spacing = ImGui::GetFrameHeightWithSpacing();

    return glm::vec2(ImGui::CalcTextSize(m_msg).x + spacing, spacing * 3.25f);
}

bool ErrorModal::Execute()
{
    ImGui::Text(m_msg);

    if (ImGui::Button("OK"))
    {
        return false;
    }

    return true;
}