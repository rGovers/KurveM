#include "Windows/OptionsWindow.h"

#include <string>

#include "imgui.h"
#include "ImGuiExt.h"
#include "Object.h"
#include "Workspace.h"

const char* EditorMode_String[] = 
{ 
    "Object Mode", 
    "Edit Mode", 
    "Weight Mode" 
};
const char* EditorMode_Path[] =
{
    "Textures/EDITOR_OBJECT.png",
    "Textures/EDITOR_EDIT.png",
    "Textures/EDITOR_WEIGHT.png"
};

#define FRONTFACE_TOOLTIP "Displays the object front faces"
#define BACKFACE_TOOLTIP "Displays the objects back faces"
#define BOTHFACE_TOOLTIP "Displays both of the objects faces"
#define NOFACE_TOOLTIP "Displays none of the objects faces" 

OptionsWindow::OptionsWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
OptionsWindow::~OptionsWindow()
{

}

void OptionsWindow::EditorFaceButton(const char* a_text, const char* a_path, e_EditorFaceCullingMode a_face, const char* a_tooltip)
{
    if (ImGuiExt::ImageToggleButton(a_text, a_path, m_editor->GetEditorFaceCullingMode() == a_face, { 16, 16 }))
    {
        m_editor->SetEditorFaceCullingMode(a_face);
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        ImGui::Text(a_text);

        ImGui::Separator();

        ImGui::Text(a_tooltip);

        ImGui::EndTooltip();
    }
}
void OptionsWindow::Update(double a_delta)
{
    if (ImGui::Begin("Options"))
    {
        const ImVec2 size = ImGui::GetWindowSize();

        ImGui::Columns(glm::max(1.0f, size.x / 256), nullptr, false);

        const e_EditorMode currentIndex = m_editor->GetEditorMode();

        if (ImGui::BeginCombo("##combo", EditorMode_String[m_editor->GetEditorMode()]))
        {
            for (int i = 0; i < EditorMode_End; ++i)
            {
                if (m_editor->IsEditorModeEnabled((e_EditorMode)i))
                {   
                    const bool selected = currentIndex == i;
                    ImGuiExt::Image(EditorMode_Path[i], { 16, 16 });

                    ImGui::SameLine();

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

        EditorFaceButton("Front Faces", "Textures/CULL_BACK.png", EditorFaceCullingMode_Back, FRONTFACE_TOOLTIP);
        ImGui::SameLine();
        EditorFaceButton("Back Faces", "Textures/CULL_FRONT.png", EditorFaceCullingMode_Front, BACKFACE_TOOLTIP);
        ImGui::SameLine();
        EditorFaceButton("Both Faces", "Textures/CULL_NONE.png", EditorFaceCullingMode_None, BOTHFACE_TOOLTIP);
        ImGui::SameLine();
        EditorFaceButton("No Faces", "Textures/CULL_ALL.png", EditorFaceCullingMode_All, NOFACE_TOOLTIP);

        ImGui::EndGroup();

        if (currentIndex == EditorMode_WeightPainting)
        {
            std::list<Object*> nodes;

            const Object* obj = m_workspace->GetSelectedObject();
            if (obj != nullptr)
            {
                const e_ObjectType type = obj->GetObjectType();
                switch (type)
                {
                case ObjectType_CurveModel:
                {
                    const CurveModel* model = obj->GetCurveModel();
                    if (model != nullptr)
                    {
                        nodes = model->GetArmatureNodes();
                    }

                    break;
                }
                }
            }

            const unsigned int size = nodes.size();
            if (size > 0)
            {
                ImGui::NextColumn();

                const long long currNode = m_editor->GetSelectedWeightNode();

                const char* selectedVal = (*nodes.begin())->GetName();
                long long selectedID = 0;

                for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
                {
                    const Object* obj = *iter;

                    if (obj->GetID() == currNode)
                    {
                        selectedVal = obj->GetName();
                        selectedID = obj->GetID();

                        break;
                    }
                }

                if (ImGui::BeginCombo("Bone", selectedVal))
                {
                    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
                    {
                        const Object* obj = *iter;

                        const long long objID = obj->GetID();
                        const char* name = obj->GetName();

                        const std::string str = std::to_string(objID) + name + "ArmatureNode";

                        const bool selected = selectedID == objID;

                        const ImGuiID id = ImGui::GetID(str.c_str());
                        ImGui::PushID(id);

                        const bool clicked = ImGui::Selectable(name, selected);

                        ImGui::PopID();

                        if (clicked)
                        {
                            m_editor->SetSelectedWeightNode(*iter);
                        }

                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::EndCombo();
                }
            }

            ImGui::NextColumn();

            ImGui::BeginGroup();

            float brushRadius = m_editor->GetBrushRadius();
            if (ImGui::DragFloat("Brush Radius", &brushRadius, 0.01))
            {
                m_editor->SetBrushRadius(brushRadius);
            }

            float brushIntensity = m_editor->GetBrushIntensity();
            if (ImGui::SliderFloat("Intensity", &brushIntensity, -1.0f, 1.0f))
            {
                m_editor->SetBrushIntensity(brushIntensity);
            }

            ImGui::EndGroup();   
        }

        ImGui::Columns();
    }

    ImGui::End();
}