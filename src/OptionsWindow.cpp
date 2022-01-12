#include "Windows/OptionsWindow.h"

#include <string>

#include "Actions/DeleteAnimationAction.h"
#include "Actions/SetAnimationFramerateAction.h"
#include "Actions/SetAnimationLengthAction.h"
#include "Animation.h"
#include "CurveModel.h"
#include "imgui.h"
#include "ImGuiExt.h"
#include "Modals/ConfirmModal.h"
#include "Modals/CreateAnimationModal.h"
#include "Object.h"
#include "Workspace.h"

const char* EditorMode_String[] = 
{ 
    "Object Mode", 
    "Edit Mode", 
    "Weight Mode",
    "Animate Mode"
};
const char* EditorMode_Path[] =
{
    "Textures/EDITOR_OBJECT.png",
    "Textures/EDITOR_EDIT.png",
    "Textures/EDITOR_WEIGHT.png",
    "Textures/EDITOR_ANIMATE.png"
};

const char* EditorMode_Preview_Path[] =
{
    "Textures/EDITOR_OBJECT_DARK.png",
    "Textures/EDITOR_EDIT_DARK.png",
    "Textures/EDITOR_WEIGHT_DARK.png",
    "Textures/EDITOR_ANIMATE_DARK.png"
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
    if (ImGuiExt::ImageToggleButton(a_text, a_path, m_editor->GetEditorFaceCullingMode() == a_face, glm::vec2(16, 16)))
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

void OptionsWindow::DeleteAnimation(bool a_state)
{
    if (a_state)
    {
        Action* action = new DeleteAnimationAction(m_workspace, m_workspace->GetCurrentAnimation());
        if (!m_workspace->PushAction(action))
        {
            printf("Failed to delete animation \n");
            
            delete action;
        }
    }
}

void OptionsWindow::Update(double a_delta)
{
    if (ImGui::Begin("Options"))
    {
        const ImVec2 size = ImGui::GetWindowSize();

        ImGui::Columns(glm::max(1.0f, size.x / 256), nullptr, false);

        const e_EditorMode currentIndex = m_editor->GetEditorMode();

        if (ImGuiExt::BeginImageCombo("##combo", EditorMode_Preview_Path[currentIndex], glm::vec2(16, 16), EditorMode_String[currentIndex]))
        {
            for (int i = 0; i < EditorMode_End; ++i)
            {
                if (m_editor->IsEditorModeEnabled((e_EditorMode)i))
                {   
                    const bool selected = currentIndex == i;
                    if (ImGuiExt::Image(EditorMode_Path[i], glm::vec2(16, 16)))
                    {
                        ImGui::SameLine();
                    }

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

        switch (currentIndex)
        {
        case EditorMode_Animate:
        {
            ImGui::NextColumn();

            if (ImGui::Button("Create Animation"))
            {
                m_workspace->PushModal(new CreateAnimationModal(m_workspace));
            }

            const std::list<Animation*> animations = m_workspace->GetAnimations();

            if (animations.size() > 0)
            {
                ImGui::NextColumn();

                Animation* curAnimation = m_workspace->GetCurrentAnimation();

                const char* curAnimName = "";
                if (curAnimation != nullptr)
                {
                    curAnimName = curAnimation->GetName();
                }

                if (ImGui::BeginCombo("Current Animation", curAnimName))
                {
                    for (auto iter = animations.begin(); iter != animations.end(); ++iter)
                    {
                        Animation* animation = *iter;

                        const bool selected = animation == curAnimation;

                        if (ImGui::Selectable(animation->GetName(), selected))
                        {
                            m_workspace->SetCurrentAnimation(animation);
                        }

                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::EndCombo();
                }

                if (curAnimation != nullptr)
                {
                    ImGui::NextColumn();

                    if (ImGui::Button("Delete Animation"))
                    {
                        m_workspace->PushModal(new ConfirmModal("Delete Animation", std::bind(&OptionsWindow::DeleteAnimation, this, std::placeholders::_1)));
                    }

                    ImGui::NextColumn();

                    int refFrameRate = curAnimation->GetReferenceFramerate();
                    if (ImGui::InputInt("Reference Framerate", &refFrameRate))
                    {
                        switch (m_workspace->GetCurrentActionType())
                        {
                        case ActionType_SetAnimationFramerate:
                        {
                            SetAnimationFramerateAction* action = (SetAnimationFramerateAction*)m_workspace->GetCurrentAction();

                            action->SetFramerate(glm::max(1, refFrameRate));
                            action->Execute();

                            break;
                        }
                        default:
                        {
                            Action* action = new SetAnimationFramerateAction(curAnimation, glm::max(1, refFrameRate));
                            if (!m_workspace->PushAction(action))
                            {
                                printf("Cannot set animation length \n");

                                delete action;
                            }
                            else
                            {
                                m_workspace->SetCurrentAction(action);
                            }

                            break;
                        }
                        }
                    }

                    ImGui::NextColumn();

                    float animTime = curAnimation->GetAnimationLength();
                    if (ImGui::DragFloat("Animation Length", &animTime, 0.01f))
                    {
                        switch (m_workspace->GetCurrentActionType())
                        {
                        case ActionType_SetAnimationLength:
                        {
                            SetAnimationLengthAction* action = (SetAnimationLengthAction*)m_workspace->GetCurrentAction();

                            action->SetLength(glm::max(0.0f, animTime));
                            action->Execute();

                            break;
                        }
                        default:
                        {
                            Action* action = new SetAnimationLengthAction(curAnimation, glm::max(0.0f, animTime));
                            if (!m_workspace->PushAction(action))
                            {
                                printf("Cannot set animation length \n");

                                delete action;
                            }
                            else
                            {
                                m_workspace->SetCurrentAction(action);
                            }

                            break;
                        }
                        }
                    }
                }
            }

            break;
        }
        case EditorMode_WeightPainting:
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

            break;
        }
        }

        ImGui::Columns();
    }

    ImGui::End();
}