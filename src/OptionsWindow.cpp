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
#include "PathModel.h"
#include "Workspace.h"

OptionsWindow::OptionsWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
OptionsWindow::~OptionsWindow()
{

}

void OptionsWindow::EditorFaceButton(e_EditorFaceCullingMode a_face)
{
    const char* name = DisplayFaceName[a_face];

    if (ImGuiExt::ImageToggleButton(name, DisplayFacePaths[a_face], m_editor->GetEditorFaceCullingMode() == a_face, glm::vec2(16.0f)))
    {
        m_editor->SetEditorFaceCullingMode(a_face);
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        ImGui::Text(name);

        ImGui::Separator();

        ImGui::Text(DisplayFaceTooltip[a_face]);

        ImGui::EndTooltip();
    }
}
void OptionsWindow::EditorDrawButton(e_EditorDrawMode a_mode)
{
    const char* name = DrawName[a_mode];

    if (ImGuiExt::ImageToggleButton(name, "", m_editor->GetEditorDrawMode() == a_mode, glm::vec2(16.0f)))
    {
        m_editor->SetEditorDrawMode(a_mode);
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        ImGui::Text(name);

        ImGui::Separator();

        ImGui::Text(DrawTooltip[a_mode]);

        ImGui::EndTooltip();
    }
}

void OptionsWindow::EditorMirrorButton(const char* a_text, const char* a_path, e_MirrorMode a_mode, const char* a_tooltip)
{
    e_MirrorMode mode = m_editor->GetMirrorMode();

    const bool active = (mode & a_mode) != 0;

    if (ImGuiExt::ImageToggleButton(a_text, a_path, active, glm::vec2(16.0f)))
    {
        if (active)
        {
            mode = (e_MirrorMode)(mode & ~a_mode);
        }
        else
        {
            mode = (e_MirrorMode)(mode | a_mode);
        }

        m_editor->SetMirrorMode(mode);
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

        if (ImGuiExt::BeginImageCombo("##combo", EditorModePreviewPath[currentIndex], glm::vec2(16.0f), EditorModeString[currentIndex]))
        {
            for (int i = 0; i < EditorMode_End; ++i)
            {
                if (m_editor->IsEditorModeEnabled((e_EditorMode)i))
                {   
                    const bool selected = currentIndex == i;
                    if (ImGuiExt::Image(EditorModePath[i], glm::vec2(16.0f)))
                    {
                        ImGui::SameLine();
                    }

                    if (ImGui::Selectable(EditorModeString[i], selected))
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

        for (int i = 0; i < EditorFaceCullingMode_End; ++i)
        {
            EditorFaceButton((e_EditorFaceCullingMode)i);
            ImGui::SameLine();
        }

        ImGui::EndGroup();

        ImGui::NextColumn();

        ImGui::BeginGroup();

        for (int i = 0; i < EditorDrawMode_End; ++i)
        {
            EditorDrawButton((e_EditorDrawMode)i);
            ImGui::SameLine();
        }

        ImGui::EndGroup();

        switch (currentIndex)
        {
        case EditorMode_Edit:
        {
            ImGui::NextColumn();

            ImGui::BeginGroup();

            EditorMirrorButton("X", "Textures/MIRROR_X.png", MirrorMode_X, MirrorXTooltip);
            ImGui::SameLine();
            EditorMirrorButton("Y", "Textures/MIRROR_Y.png", MirrorMode_Y, MirrorYTooltip);
            ImGui::SameLine();
            EditorMirrorButton("Z", "Textures/MIRROR_Z.png", MirrorMode_Z, MirrorZTooltip);

            ImGui::EndGroup();

            break;
        }
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
                case ObjectType_PathModel:
                {
                    const PathModel* model = obj->GetPathModel();
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