#include "Windows/EditorWindow.h"

#include "EditorControls/Editor.h"
#include "imgui.h"
#include "ImGuiExt.h"
#include "Object.h"
#include "RenderTexture.h"
#include "Texture.h"
#include "Workspace.h"

EditorWindow::EditorWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
EditorWindow::~EditorWindow()
{

}

void EditorWindow::Update(double a_delta)
{
    if (ImGui::Begin("Editor"))
    {
        const ImVec2 pos = ImGui::GetWindowPos();
        const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        const ImVec2 size = { vMax.x - vMin.x, vMax.y - vMin.y };

        const glm::vec2 winPos = glm::vec2(pos.x + vMin.x, pos.y + vMin.y);
        const glm::vec2 winSize = glm::vec2(size.x, size.y);

        m_editor->Update(a_delta, winPos, winSize);

        const RenderTexture* renderTexture = m_editor->GetRenderTexture();
        const Texture* texture = renderTexture->GetTexture();

        ImGui::Image((ImTextureID)texture->GetHandle(), size);

        if (ImGui::BeginPopupContextWindow())
        {
            m_editor->UpdateContextMenu(winPos, winSize);

            ImGui::EndPopup();
        }
    }
    ImGui::End();
}