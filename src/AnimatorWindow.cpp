#include "Windows/AnimatorWindow.h"

#include <string>

#include "Actions/AddAnimationNodeAction.h"
#include "Actions/RemoveAnimationNodeAction.h"
#include "Animation.h"
#include "ColorTheme.h"
#include "EditorControls/Editor.h"
#include "imgui.h"
#include "ImGuiExt.h"
#include "Object.h"
#include "PhysicsEngine.h"
#include "Workspace.h"

AnimatorWindow::AnimatorWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_playing = false;
}
AnimatorWindow::~AnimatorWindow()
{

}

void AnimatorWindow::Update(double a_delta)
{
    if (m_editor->GetEditorMode() == EditorMode_Animate)
    {
        const ImGuiStyle& style = ImGui::GetStyle();

        if (ImGui::Begin("Animator"))
        {
            Animation* animation = m_workspace->GetCurrentAnimation();
            if (animation != nullptr)
            {
                const float time = m_editor->GetSelectedTime();

                const int refFrameRate = animation->GetReferenceFramerate();
                const float animationLength = animation->GetAnimationLength();

                const int animLength = refFrameRate * animationLength;
                const float animStep = 1.0f / refFrameRate;

                if (ImGuiExt::ImageButton("Start", "Textures/ANIMATOR_START.png", glm::vec2(16.0f)))
                {
                    m_workspace->ResetAnimationObjects();
                    
                    m_editor->SetSelectedTime(0.0f);
                    m_editor->SetAnimationTime(0.0f);
                }
                ImGui::SameLine();
                if (ImGuiExt::ImageSwitchButton("Play", "Textures/ANIMATOR_STOP.png", "Textures/ANIMATOR_PLAY.png", &m_playing, glm::vec2(16.0f), true))
                {
                    if (!m_playing)
                    {
                        m_workspace->ResetAnimationObjects();
                    }
                    
                    m_editor->SetAnimationTime(time);
                }
                ImGui::SameLine();
                if (ImGuiExt::ImageButton("End", "Textures/ANIMATOR_END.png", glm::vec2(16.0f)))
                {
                    m_workspace->ResetAnimationObjects();
                    
                    m_editor->SetSelectedTime(animationLength);
                    m_editor->SetAnimationTime(animationLength);
                }

                if (m_playing)
                {
                    double time = m_editor->GetAnimationTime() + a_delta;
                    if (time > animationLength)
                    {
                        time -= animationLength;

                        m_workspace->ResetAnimationObjects();
                    }

                    m_editor->SetAnimationTime((float)time);
                    m_editor->GetPhysicsEngine()->Update(a_delta);
                }

                if (ImGui::BeginChild("Outter Frame", { 0.0f, 0.0f }, false, ImGuiWindowFlags_AlwaysVerticalScrollbar))
                {
                    const float frameHeight = ImGui::GetTextLineHeight();

                    ImGui::BeginGroup();

                    ImGui::Dummy({ 256.0f, frameHeight });

                    const std::list<Object*> objs = m_workspace->GetAllObjectsList();
                    std::list<AnimationGroup> animationNodes = animation->GetNodes();

                    for (auto iter = objs.begin(); iter != objs.end(); ++iter)
                    {
                        Object* obj = *iter;
                        const long long objectID = obj->GetID();

                        const AnimationGroup* group = nullptr; 

                        for (auto innerIter = animationNodes.begin(); innerIter != animationNodes.end(); ++innerIter)
                        {
                            if (innerIter->ObjectID == objectID)
                            {
                                group = &*innerIter;

                                break;
                            }
                        }

                        switch (obj->GetObjectType())
                        {
                        case ObjectType_ArmatureNode:
                        {
                            const char* name = obj->GetName();

                            if (name == nullptr)
                            {
                                name = "NULL";
                            }

                            ImGui::Text(name);

                            ImGui::SameLine();

                            ImGui::SetCursorPosX(256 - style.ItemSpacing.x - 16);

                            char* idName = obj->GetIDName();

                            bool keyFrame = false;
                            AnimationNode nodeD;

                            if (group != nullptr)
                            {
                                for (auto innerIter = group->Nodes.begin(); innerIter != group->Nodes.end(); ++innerIter)
                                {
                                    if (time <= innerIter->Time && time + animStep > innerIter->Time)
                                    {
                                        keyFrame = true;
                                        nodeD = *innerIter;

                                        break;
                                    }
                                }
                            }
                            
                            if (!keyFrame)
                            {
                                const std::string btnStr = std::string(idName) + "AddNode";

                                const ImGuiID id = ImGui::GetID(btnStr.c_str());

                                ImGui::PushID(id);
                                const bool addPressed = ImGuiExt::ImageButton("+", "Textures/ANIMATOR_ADDKEYFRAME.png", glm::vec2(16));
                                ImGui::PopID();

                                if (addPressed)
                                {
                                    AnimationNode node;
                                    node.Time = time;
                                    node.Translation = animation->GetTranslation(*iter, time);
                                    node.Rotation = animation->GetRotation(*iter, time);
                                    node.Scale = animation->GetScale(*iter, time);

                                    Action* action = new AddAnimationNodeAction(animation, *iter, node);
                                    if (!m_workspace->PushAction(action))
                                    {
                                        printf("Failed to create keyframe \n");

                                        delete action;
                                    }
                                }
                            }
                            else
                            {
                                const std::string btnStr = std::string(idName) + "RemoveNode";

                                const ImGuiID id = ImGui::GetID(btnStr.c_str());

                                ImGui::PushID(id);
                                const bool removePressed = ImGuiExt::ImageButton("-", "Textures/ANIMATOR_REMOVEKEYFRAME.png", glm::vec2(16));
                                ImGui::PopID();

                                if (removePressed)
                                {
                                    Action* action = new RemoveAnimationNodeAction(animation, *iter, nodeD);
                                    if (!m_workspace->PushAction(action))
                                    {
                                        printf("Failed to remove keyframe \n");

                                        delete action;
                                    }
                                }
                            }
                            
                            delete[] idName;

                            break;
                        }
                        }
                    }

                    ImGui::EndGroup();

                    ImGui::SameLine();

                    if (ImGui::BeginChild("Inner Frame", { 0.0f, 0.0f }, false, ImGuiWindowFlags_AlwaysHorizontalScrollbar))
                    {
                        const int timeStep = animationLength / 0.5f;
                        const float referenceStep = refFrameRate * 0.5f;
                        const float stepSize = referenceStep * 20 + style.ItemSpacing.x * referenceStep;
                        float scrollTime = time;
                        if (m_playing)
                        {
                            scrollTime = m_editor->GetAnimationTime();
                        }

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

                        for (auto iter = objs.begin(); iter != objs.end(); ++iter)
                        {
                            Object* obj = *iter;
                            const long long objID = obj->GetID();

                            AnimationGroup* group = nullptr; 

                            for (auto innerIter = animationNodes.begin(); innerIter != animationNodes.end(); ++innerIter)
                            {
                                if (innerIter->ObjectID == objID)
                                {
                                    group = &*innerIter;

                                    break;
                                }
                            }

                            char* name = obj->GetIDName();

                            switch (obj->GetObjectType())
                            {
                            case ObjectType_ArmatureNode:
                            {
                                std::list<AnimationNode>::iterator animNodeIter;

                                if (group != nullptr)
                                {
                                    animNodeIter = group->Nodes.begin();
                                }

                                for (int i = 0; i <= animLength; ++i)
                                {
                                    const float curTime = i * animStep;
                                    const float nextTime = (i + 1) * animStep;

                                    glm::vec4 color = glm::vec4(0, 0, 0, 1);

                                    if (curTime <= scrollTime && nextTime > scrollTime)
                                    {
                                        color = ColorTheme::InActive;
                                    }

                                    if (group != nullptr)
                                    {
                                        // Had to step therefore at a keyframe
                                        while (animNodeIter != group->Nodes.end() && curTime >= animNodeIter->Time)
                                        {
                                            color = ColorTheme::Active;

                                            ++animNodeIter;
                                        }
                                    }
                                    
                                    const std::string buttonStr = "##" + std::to_string(i) + name;

                                    if (ImGuiExt::ColoredButton(buttonStr.c_str(), color, glm::vec2(20)))
                                    {
                                        m_workspace->ResetAnimationObjects();

                                        m_workspace->ClearSelectedObjects();
                                        m_workspace->AddSelectedObject(obj);

                                        m_editor->SetSelectedTime(curTime);
                                        m_editor->SetAnimationTime(curTime);
                                    }

                                    if (i + 1 <= animLength)
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