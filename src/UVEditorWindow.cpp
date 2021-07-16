#include "Windows/UVEditorWindow.h"

#include "Editor.h"
#include "imgui.h"
#include "RenderTexture.h"
#include "Texture.h"
#include "UVEditor.h"

UVEditorWindow::UVEditorWindow(Workspace* a_workspace, Editor* a_editor, UVEditor* a_uvEditor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
    m_uvEditor = a_uvEditor;
}
UVEditorWindow::~UVEditorWindow()
{

}

void UVEditorWindow::Update(double a_delta)
{
    bool open = m_editor->GetEditorMode() == EditorMode_Edit;
    if (open)
    {
        if (ImGui::Begin("UV Editor", nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
        {
            const ImVec2 pos = ImGui::GetWindowPos();
            const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
            const ImVec2 size = { vMax.x - vMin.x, vMax.y - vMin.y };

            m_uvEditor->Update(a_delta, { pos.x + vMin.x, pos.y + vMin.y }, { size.x, size.y });

            const RenderTexture* renderTexture = m_uvEditor->GetRenderTexture();
            const Texture* texture = renderTexture->GetTexture();

            ImGui::Image((ImTextureID)texture->GetHandle(), size);
        }
        ImGui::End();
    }   
}