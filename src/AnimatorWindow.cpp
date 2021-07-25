#include "Windows/AnimatorWindow.h"

#include "Animation.h"
#include "ColorTheme.h"
#include "Editor.h"
#include "imgui.h"
#include "ImGuiExt.h"
#include "Object.h"
#include "Workspace.h"

AnimatorWindow::AnimatorWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
AnimatorWindow::~AnimatorWindow()
{

}

void AnimatorWindow::Update(double a_delta)
{
    if (m_editor->GetEditorMode() == EditorMode_Animate)
    {
        if (ImGui::Begin("Animator"))
        {
            Animation* animation = m_workspace->GetCurrentAnimation();
            if (animation != nullptr)
            {
                const float time = m_editor->GetSelectedTime();

                const int refFrameRate = animation->GetReferenceFramerate();
                const float animationLength = animation->GetAnimationLength();

                const int animLength = refFrameRate * animationLength;

                if (ImGui::BeginChild("Outter Frame", {0, 0}, false, ImGuiWindowFlags_AlwaysVerticalScrollbar))
                {
                    const float frameHeight = ImGui::GetTextLineHeight();

                    ImGui::BeginGroup();

                    ImGui::Dummy({ 256, frameHeight });

                    const std::list<Object *> objs = m_workspace->GetAllObjectsList();

                    for (auto iter = objs.begin(); iter != objs.end(); ++iter)
                    {
                        const Object *obj = *iter;

                        switch (obj->GetObjectType())
                        {
                        case ObjectType_ArmatureNode:
                        {
                            const char *name = obj->GetName();

                            if (name == nullptr)
                            {
                                name = "NULL";
                            }

                            ImGui::Text(name);

                            break;
                        }
                        }
                    }

                    ImGui::EndGroup();

                    ImGui::SameLine();

                    if (ImGui::BeginChild("Inner Frame", { 0, 0 }, false, ImGuiWindowFlags_AlwaysHorizontalScrollbar))
                    {
                        const ImGuiStyle style = ImGui::GetStyle();

                        const int timeStep = animationLength / 0.5f;
                        const float referenceStep = refFrameRate * 0.5f;
                        const float stepSize = referenceStep * frameHeight + style.ItemSpacing.x * referenceStep;

                        for (int i = 0; i <= timeStep; ++i)
                        {
                            char tOut[128];
                            sprintf(tOut, "%.1f", i * 0.5f);

                            ImGui::SetCursorPosX(i * stepSize);

                            ImGui::Text(tOut);

                            if (i < timeStep)
                            {
                                ImGui::SameLine();
                            }
                        }

                        const float animStep = 1.0f / refFrameRate;

                        for (auto iter = objs.begin(); iter != objs.end(); ++iter)
                        {
                            const Object* obj = *iter;

                            char* name = obj->GetIDName();

                            switch (obj->GetObjectType())
                            {
                            case ObjectType_ArmatureNode:
                            {
                                for (int i = 0; i < animLength; ++i)
                                {
                                    const float curTime = i * animStep;

                                    glm::vec4 color = glm::vec4(0, 0, 0, 1);

                                    if (curTime <= time && (i + 1) * animStep > time)
                                    {
                                        color = ColorTheme::InActive;
                                    }

                                    const std::string str = "##" + std::to_string(i) + name;

                                    if (ImGuiExt::ColoredButton(str.c_str(), color, glm::vec2(frameHeight, frameHeight)))
                                    {
                                        m_editor->SetSelectedTime(curTime);
                                    }

                                    if (i + 1 < animLength)
                                    {
                                        ImGui::SameLine();
                                    }
                                }

                                break;
                            }
                            }

                            delete[] name;
                        }
                    }

                    ImGui::EndChild();
                }
                
                ImGui::EndChild();
            }
        }
        ImGui::End();
    }
}