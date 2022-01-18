#include "Windows/ShapeEditorWindow.h"

#include "EditorControls/Editor.h"
#include "EditorControls/ShapeEditor.h"
#include "Object.h"
#include "RenderTexture.h"
#include "Texture.h"
#include "Workspace.h"

ShapeEditorWindow::ShapeEditorWindow(Workspace* a_workspace, Editor* a_editor, ShapeEditor* a_shapeEditor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_shapeEditor = a_shapeEditor;
}
ShapeEditorWindow::~ShapeEditorWindow()
{

}

void ShapeEditorWindow::Update(double a_delta)
{
    if (m_editor->GetEditorMode() == EditorMode_Edit && m_workspace->GetSelectedObjectType() == ObjectType_PathModel)
    {
        if (ImGui::Begin("Shape Editor", nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
        {
            const ImVec2 pos = ImGui::GetWindowPos();
            const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
            const ImVec2 size = { vMax.x - vMin.x, vMax.y - vMin.y };

            m_shapeEditor->Update(a_delta, { pos.x + vMin.x, pos.y + vMin.y }, { size.x, size.y });

            const RenderTexture* renderTexture = m_shapeEditor->GetRenderTexture();
            const Texture* texture = renderTexture->GetTexture();

            ImGui::Image((ImTextureID)texture->GetHandle(), size);
        }

        ImGui::End();
    }
}