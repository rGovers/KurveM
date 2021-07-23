#include "Windows/ToolbarWindow.h"

#include "imgui.h"
#include "ImGuiExt.h"

#define TRANSLATE_TOOLTIP "Moves the selected object/node"
#define ROTATE_TOOLTIP "Rotates the selected object/node"
#define SCALE_TOOLTIP "Scales the selected object/node"

#define EXTRUDE_TOOLTIP "Extrudes nodes from existing nodes"

ToolbarWindow::ToolbarWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
ToolbarWindow::~ToolbarWindow()
{

}

void ToolbarWindow::ToolbarButton(const char* a_text, const char* a_path, e_ToolMode a_toolMode, const char* a_tooltip)
{
    if (ImGuiExt::ImageToggleButton(a_text, a_path, m_workspace->GetToolMode() == a_toolMode, { 32, 32 }))
    {
        m_workspace->SetToolMode(a_toolMode);
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        ImGui::Text(a_text);

        ImGui::Separator();

        ImGui::Text(a_tooltip);

        ImGui::EndTooltip();
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
        
        ToolbarButton("Translate", "Textures/TRANSFORM_TRANSLATE.png", ToolMode_Translate, TRANSLATE_TOOLTIP);
        ToolbarButton("Rotate", "Textures/TRANSFORM_ROTATE.png", ToolMode_Rotate, ROTATE_TOOLTIP);
        ToolbarButton("Scale", "Textures/TRANSFORM_SCALE.png", ToolMode_Scale, SCALE_TOOLTIP);

        ImGui::Columns();
        ImGui::EndGroup();

        if (m_editor->GetEditorMode() == EditorMode_Edit)
        {
            ImGui::Separator();

            ImGui::BeginGroup();
            ImGui::Columns(columns, nullptr, false);

            ToolbarButton("Extrude", "Textures/TOOLBAR_EXTRUDE.png", ToolMode_Extrude, EXTRUDE_TOOLTIP);

            ImGui::Columns();
            ImGui::EndGroup();
        }
    }

    ImGui::End();
}