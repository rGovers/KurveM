#include "Windows/HierarchyWindow.h"

#include "Actions/CreateObjectAction.h"
#include "Actions/DeleteObjectAction.h"
#include "imgui.h"
#include "Object.h"
#include "Workspace.h"

HierarchyWindow::HierarchyWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
HierarchyWindow::~HierarchyWindow()
{

}

bool HierarchyWindow::ObjectHeirachyGUI(Object* a_object, bool* a_blockMenu)
{
    int id = a_object->GetID();

    bool selected = false;

    *a_blockMenu = false;

    const std::list<Object*> selectedObjects = m_workspace->GetSelectedObjects();

    for (auto iter = selectedObjects.begin(); iter != selectedObjects.end(); ++iter)
    {
        if ((*iter)->GetID() == a_object->GetID())
        {
            selected = true;

            break;
        }
    }

    bool open = ImGui::TreeNode((void*)&id, "");

    ImGui::SameLine();

    const char* name = a_object->GetName();
    if (name == nullptr)
    {
        name = "NULL";
    }

    if (ImGui::Selectable(("[" + std::to_string(a_object->GetID()) + "] " + name).c_str(), selected))
    {
        m_workspace->ClearCurrentAction();

        ImGuiIO& io = ImGui::GetIO();

        if (io.KeyCtrl)
        {
            bool found = false;

            for (auto iter = m_workspace->GetSelectedObjects().begin(); iter != m_workspace->GetSelectedObjects().end(); ++iter)
            {
                if ((*iter)->GetID() == a_object->GetID())
                {
                    m_workspace->RemoveSelectedObject(*iter);

                    found = true;

                    break;
                }
            }

            if (!found)
            {
                m_workspace->AddSelectedObject(a_object);
            }
        }
        else
        {
            m_workspace->ClearSelectedObjects();

            m_workspace->AddSelectedObject(a_object);
        }
    }

    if (selected)
    {
        ImGui::SetItemDefaultFocus();
    }

    bool ret = false;

    if (ImGui::BeginPopupContextItem())
    {
        *a_blockMenu = true;

        if (ImGui::MenuItem("New Object"))
        {
            Action* action = new CreateObjectAction(m_workspace, a_object);
            if (!m_workspace->PushAction(action))
            {
                printf("Error Creating Object \n");

                delete action;
            }
        }

        ImGui::Separator();

        m_workspace->CreateCurveObjectMenuList(a_object);

        ImGui::Separator();

        m_workspace->ImportObjectMenuList(a_object);

        ImGui::Separator();

        if (ImGui::MenuItem("Delete Object"))
        {
            Action* action = new DeleteObjectAction(m_workspace, a_object);
            if (!m_workspace->PushAction(action))
            {
                printf("Error Deleting Object \n");

                delete action;
            }

            ret = true;
        }

        ImGui::EndPopup();
    }

    if (open)
    {
        const std::list<Object*> children = a_object->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            bool val;

            if (ObjectHeirachyGUI(*iter, &val))
            {
                ret = true;
            }

            if (val)
            {
                *a_blockMenu = true;
            }
        }

        ImGui::TreePop();
    }

    return ret;
}

void HierarchyWindow::Update(double a_delta)
{
    bool blk = false;
    if (ImGui::Begin("Hierarchy"))
    {
        const std::list<Object*> objs = m_workspace->GetObjectList();

        for (auto iter = objs.begin(); iter != objs.end(); ++iter)
        {
            bool val;

            if (ObjectHeirachyGUI(*iter, &val))
            {
                break;
            }

            if (val)
            {
                blk = true;
            }
        }

        if (!blk && ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("New Object"))
            {
                Action* action = new CreateObjectAction(m_workspace, nullptr);
                if (!m_workspace->PushAction(action))
                {
                    printf("Error Creating Object \n");

                    delete action;
                }
            }

            ImGui::Separator();

            m_workspace->CreateCurveObjectMenuList(nullptr);

            ImGui::Separator();

            m_workspace->ImportObjectMenuList(nullptr);

            ImGui::EndPopup();
        }
    }

    ImGui::End();
}