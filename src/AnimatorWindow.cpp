#include "Windows/AnimatorWindow.h"

#include "imgui.h"

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
    if (ImGui::Begin("Animator"))
    {

    }
    ImGui::End();
}