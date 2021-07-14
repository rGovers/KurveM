#include "Windows/ToolbarWindow.h"

#include "imgui.h"
#include "ImGuiExt.h"

ToolbarWindow::ToolbarWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
ToolbarWindow::~ToolbarWindow()
{

}

void ToolbarWindow::ToolbarButton(const char* a_text, e_ToolMode a_toolMode)
{
    if (ImGuiExt::ToggleButton(a_text, m_workspace->GetToolMode() == a_toolMode, { 32, 32 }))
    {
        m_workspace->SetToolMode(a_toolMode);
    }

    ImGui::NextColumn();
}

void ToolbarWindow::Update(double a_delta)
{
    if (ImGui::Begin("Toolbar"))
    {
        const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        const glm::vec2 size = { vMax.x - vMin.x, vMax.y - vMin.y };

        const int columns = glm::max(1, (int)glm::floor(size.x / 36.0f));

        ImGui::BeginGroup();
        ImGui::Columns(columns, nullptr, false);
        
        ToolbarButton("Translate", ToolMode_Translate);
        ToolbarButton("Rotate", ToolMode_Rotate);
        ToolbarButton("Scale", ToolMode_Scale);

        ImGui::Columns();
        ImGui::EndGroup();

        if (m_editor->GetEditorMode() == EditorMode_Edit)
        {
            ImGui::Separator();

            ImGui::BeginGroup();
            ImGui::Columns(columns, nullptr, false);

            ToolbarButton("Extrude", ToolMode_Extrude);

            ImGui::Columns();
            ImGui::EndGroup();
        }
    }

    ImGui::End();
}