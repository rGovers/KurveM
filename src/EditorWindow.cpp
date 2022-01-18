#include "Windows/EditorWindow.h"

#include "Actions/CreateObjectAction.h"
#include "Actions/FlipFaceAction.h"
#include "Actions/InsertFaceAction.h"
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

        m_editor->Update(a_delta, { pos.x + vMin.x, pos.y + vMin.y }, { size.x, size.y });

        const RenderTexture* renderTexture = m_editor->GetRenderTexture();
        const Texture* texture = renderTexture->GetTexture();

        ImGui::Image((ImTextureID)texture->GetHandle(), size);

        if (ImGui::BeginPopupContextWindow())
        {
            switch (m_editor->GetEditorMode())
            {
            case EditorMode_Object:
            {
                m_workspace->CreateCurveObjectMenuList(nullptr);

                ImGui::Separator();

                m_workspace->CreatePathObjectMenuList(nullptr);

                ImGui::Separator();

                m_workspace->ImportObjectMenuList(nullptr);

                ImGui::Separator();

                ImGuiExt::Image("Textures/OBJECT_ARMATURE.png", glm::vec2(16, 16));

                ImGui::SameLine();

                if (ImGui::MenuItem("New Armature"))
                {
                    Action* action = new CreateObjectAction(m_workspace, nullptr, CreateObjectType_Armature);
                    if (!m_workspace->PushAction(action))
                    {
                        printf("Error Creating Armature \n");

                        delete action;
                    }
                }

                break;
            }
            case EditorMode_Edit:
            {
                if (m_editor->CanInsertFace() && ImGui::MenuItem("Insert Face"))
                {
                    Object* obj = m_workspace->GetSelectedObject();
                    if (obj != nullptr)
                    {
                        CurveModel* curveModel = obj->GetCurveModel();
                        if (curveModel != nullptr)
                        {
                            const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();

                            const unsigned int nodeCount = selectedNodes.size();

                            unsigned int* nodes = new unsigned int[nodeCount];

                            unsigned int index = 0;
                            for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                            {
                                nodes[index++] = *iter;
                            }

                            Action* action = new InsertFaceAction(m_workspace, nodes, nodeCount, curveModel);
                            if (!m_workspace->PushAction(action))
                            {
                                printf("Error creating face \n");

                                delete action;
                            }

                            delete[] nodes;
                        }

                        if (m_editor->IsFaceSelected() && ImGui::MenuItem("Flip Face"))
                        {
                            const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();

                            const unsigned int nodeCount = selectedNodes.size();

                            unsigned int* nodes = new unsigned int[nodeCount];

                            unsigned int index = 0;
                            for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                            {
                                nodes[index++] = *iter;
                            }

                            Action* action = new FlipFaceAction(m_workspace, nodes, nodeCount, curveModel);
                            if (!m_workspace->PushAction(action))
                            {
                                printf("Error fliping face \n");

                                delete action;
                            }

                            delete[] nodes;
                        }
                    }
                }

                ImGui::Separator();

                break;
            }
            }
            
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}