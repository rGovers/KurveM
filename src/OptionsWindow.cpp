#include "Windows/OptionsWindow.h"

#include "imgui.h"
#include "ImGuiExt.h"
#include "Workspace.h"

const char* EditorMode_String[] = { "Object Mode", "Edit Mode" };

OptionsWindow::OptionsWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
OptionsWindow::~OptionsWindow()
{

}

void OptionsWindow::EditorFaceButton(const char* a_text, e_EditorFaceCullingMode a_face)
{
    if (ImGuiExt::ToggleButton(a_text, m_editor->GetEditorFaceCullingMode() == a_face, { 32, 32 }))
    {
        m_editor->SetEditorFaceCullingMode(a_face);
    }
}
void OptionsWindow::Update(double a_delta)
{
    if (ImGui::Begin("Options"))
    {
        const ImVec2 size = ImGui::GetWindowSize();

        ImGui::Columns(size.x / 256, nullptr, false);

        const e_EditorMode currentIndex = m_editor->GetEditorMode();

        if (ImGui::BeginCombo("##combo", EditorMode_String[m_editor->GetEditorMode()]))
        {
            for (int i = 0; i < EditorMode_End; ++i)
            {
                if (m_editor->IsEditorModeEnabled((e_EditorMode)i))
                {   
                    const bool selected = currentIndex == i;
                    if (ImGui::Selectable(EditorMode_String[i], selected))
                    {
                        m_editor->SetEditorMode((e_EditorMode)i);
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::NextColumn();

        ImGui::BeginGroup();

        EditorFaceButton("Front Faces", EditorFaceCullingMode_Back);
        ImGui::SameLine();
        EditorFaceButton("Back Faces", EditorFaceCullingMode_Front);
        ImGui::SameLine();
        EditorFaceButton("Both Faces", EditorFaceCullingMode_None);
        ImGui::SameLine();
        EditorFaceButton("No Faces", EditorFaceCullingMode_All);

        ImGui::EndGroup();

        ImGui::Columns();
    }

    ImGui::End();
}