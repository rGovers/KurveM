#include "Windows/HierarchyWindow.h"

#include <string>

#include "Actions/CreateObjectAction.h"
#include "Actions/DeleteObjectAction.h"
#include "imgui.h"
#include "ImGuiExt.h"
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
    const long long id = a_object->GetID();

    bool selected = false;

    *a_blockMenu = false;

    const std::list<Object*> selectedObjects = m_workspace->GetSelectedObjects();

    for (auto iter = selectedObjects.begin(); iter != selectedObjects.end(); ++iter)
    {
        if ((*iter)->GetID() == id)
        {
            selected = true;

            break;
        }
    }

    const char* name = a_object->GetName();
    if (name == nullptr)
    {
        name = "NULL";
    }

    const std::list<Object*> children = a_object->GetChildren();

    const int childCount = children.size();

    const std::string uString = std::string("##") + name + std::to_string(id);
    
    const e_ObjectType objectType = a_object->GetObjectType(); 

    bool open = false;
    if (childCount > 0)
    {
        open = ImGui::TreeNode((uString + "Node").c_str(), "");

        ImGui::SameLine();
    }
    else 
    {
        ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
    }

    if (objectType != ObjectType_ArmatureNode)
    {
        bool visible = a_object->IsVisible();
        if (ImGuiExt::ImageSwitchButton((uString + "Visible").c_str(), "Textures/OBJECT_VISIBLE.png", "Textures/OBJECT_HIDDEN.png", &visible, { 12, 12 }))
        {
            a_object->SetVisible(visible);
        }

        ImGui::SameLine();
    }

    switch (objectType)
    {
    case ObjectType_Armature:
    {
        ImGuiExt::Image("Textures/OBJECT_ARMATURE.png", { 16, 16 });

        ImGui::SameLine();

        break;
    }
    case ObjectType_CurveModel:
    {
        ImGuiExt::Image("Textures/OBJECT_CURVE.png", { 16, 16 });

        ImGui::SameLine();

        break;
    }
    case ObjectType_ReferenceImage:
    {
        ImGuiExt::Image("Textures/OBJECT_REFERENCEIMAGE.png", { 16, 16 });

        ImGui::SameLine();

        break;
    }
    }

    const ImGuiID guiID = ImGui::GetID((uString + "Name").c_str());
    ImGui::PushID(guiID);
    bool nameClick = ImGui::Selectable(name, selected);
    ImGui::PopID();

    if (nameClick)
    {
        m_workspace->ClearCurrentAction();

        ImGuiIO& io = ImGui::GetIO();

        if (io.KeyCtrl)
        {
            bool found = false;

            const std::list<Object*> selectedObjects = m_workspace->GetSelectedObjects();

            for (auto iter = selectedObjects.begin(); iter != selectedObjects.end(); ++iter)
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

    if (objectType != ObjectType_ArmatureNode)
    {
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

            ImGuiExt::Image("Textures/OBJECT_ARMATURE.png", { 16, 16 });

            ImGui::SameLine();

            if (ImGui::MenuItem("New Armature"))
            {
                Action* action = new CreateObjectAction(m_workspace, a_object, CreateObjectType_Armature);
                if (!m_workspace->PushAction(action))
                {
                    printf("Error Creating Armature \n");

                    delete action;
                }
            }

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
    }
    
    if (open)
    {
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

    if (childCount <= 0)
    {
        ImGui::Unindent();
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

            ImGui::Separator();

            ImGuiExt::Image("Textures/OBJECT_ARMATURE.png", { 16, 16 });

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

            ImGui::EndPopup();
        }
    }

    ImGui::End();
}