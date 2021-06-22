#include "Workspace.h"

#include <stdio.h>
#include <string>
#include <string.h>

#include "Actions/Action.h"
#include "Actions/CreateObjectAction.h"
#include "Actions/DeleteObjectAction.h"
#include "Actions/RenameObjectAction.h"
#include "Application.h"
#include "CurveModel.h"
#include "Datastore.h"
#include "Editor.h"
#include "Gizmos.h"
#include "imgui_internal.h"
#include "Object.h"
#include "RenderTexture.h"
#include "Texture.h"

const char* EditorMode_String[] = { "Object Mode", "Edit Mode" };

Workspace::Workspace()
{   
    m_currentDir = nullptr;

    New();

    m_init = true;
    m_reset = false;

    m_editor = new Editor(this);
}
Workspace::~Workspace()
{
    Gizmos::Destroy();
    Datastore::Destroy();

    ClearBuffers();
}

void Workspace::ClearBuffers()
{
    for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
    {
        delete *iter;
    }
    m_objectList.clear();

    for (auto iter = m_actionStack.begin(); iter != m_actionStack.end(); ++iter)
    {
        delete *iter;
    }  
    m_actionStack.clear();

    m_actionStackIndex = m_actionStack.end();
}

void Workspace::New()
{
    ClearBuffers();

    Datastore::Destroy();
    Datastore::Init();

    Gizmos::Init();
}
void Workspace::Open(const char* a_dir)
{
    New();
}

void Workspace::Save()
{

}
void Workspace::SaveAs(const char* a_dir)
{
    if (m_currentDir != nullptr)
    {
        delete[] m_currentDir;
        m_currentDir = nullptr;
    }

    int len = strlen(a_dir) + 1;
    if (len > 0)
    {
        m_currentDir = new char[len];

        for (int i = 0; i < len; ++i)
        {
            m_currentDir[i] = a_dir[i];
        }
    }

    if (m_currentDir != nullptr)
    {
        printf(m_currentDir);
        printf("\n");

        Save();
    }
}

bool Workspace::Undo()
{
    auto iter = m_actionStackIndex;

    if (iter != m_actionStack.begin())
    {
        Action* action = *--iter;

        if (action->Revert())
        {
            m_actionStackIndex = iter;

            return true;
        }
    }

    return false;
}
bool Workspace::Redo()
{
    auto iter = m_actionStackIndex;

    if (++iter != m_actionStack.end())
    {
        Action* action = *iter;

        if (action->Redo())
        {
            m_actionStackIndex = iter;

            return true;
        }
    }

    return false;
}

bool Workspace::PushAction(Action* a_action)
{
    if (!a_action->Execute())
    {
        return false;
    }

    if (m_actionStackIndex == m_actionStack.end())
    {
        m_actionStack.emplace_back(a_action);

        m_actionStackIndex = m_actionStack.end();

        return true;
    }

    while (true)
    {
        auto iter = m_actionStackIndex;

        if (++iter == m_actionStack.end())
        {
            break;
        }

        delete *iter;

        m_actionStack.erase(iter);
    }

    m_actionStack.emplace_back(a_action);

    m_actionStackIndex = m_actionStack.end();

    while (m_actionStack.size() > 1000)
    {
        auto iter = m_actionStack.begin();

        delete *iter;

        m_actionStack.erase(iter);
    }

    return true;
}

void Workspace::AddObject(Object* a_object)
{
    for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
    {
        if ((*iter)->GetID() == a_object->GetID())
        {
            return;
        }
    }

    m_objectList.emplace_back(a_object);
}
void Workspace::RemoveObject(Object* a_object)
{
    for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
    {
        if ((*iter)->GetID() == a_object->GetID())
        {
            m_objectList.erase(iter);

            return;
        }
    }
}

void Workspace::DefaultWindowConfig()
{
    const Application* app = Application::GetInstance();

    const int windowXPos = app->GetXPos();
    const int windowYPos = app->GetYPos();

    const int windowWidth = app->GetWidth();
    const int windowHeight = app->GetHeight();

    const ImGuiID id = ImGui::GetID("Dock Main");

    ImGui::DockBuilderRemoveNode(id);
    ImGui::DockBuilderAddNode(id, ImGuiDockNodeFlags_CentralNode);

    ImGui::DockBuilderSetNodePos(id, { windowXPos, windowYPos + m_barSize });
    ImGui::DockBuilderSetNodeSize(id, { windowWidth, windowHeight - m_barSize});

    const float topSideScale = 0.1f;
    const float leftSideScale = 0.1f;
    const float rightSideScale = 0.15f;

    ImGuiID dockMainID = id;
    const ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.2f, nullptr, &dockMainID);
    ImGuiID dockRightTop = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, rightSideScale, nullptr, &dockMainID);
    const ImGuiID dockRightBottom = ImGui::DockBuilderSplitNode(dockRightTop, ImGuiDir_Down, 0.25f, nullptr, &dockRightTop);
    const ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Up, topSideScale, nullptr, &dockMainID);
    const ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, leftSideScale, nullptr, &dockMainID);

    ImGui::DockBuilderDockWindow("Options", dockTop);

    ImGui::DockBuilderDockWindow("Editor", dockMainID);

    ImGui::DockBuilderDockWindow("Hierarchy", dockRightTop);
    ImGui::DockBuilderDockWindow("Properties", dockRightBottom);

    ImGui::DockBuilderDockWindow("Toolbar", dockLeft);

    ImGui::DockBuilderFinish(id);
}

bool Workspace::ObjectHeirachyGUI(Object* a_object, bool* a_blockMenu)
{
    int id = a_object->GetID();

    bool selected = false;

    *a_blockMenu = false;

    for (auto iter = m_selectedObjects.begin(); iter != m_selectedObjects.end(); ++iter)
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
        ImGuiIO& io = ImGui::GetIO();

        if (io.KeyCtrl)
        {
            bool found = false;

            for (auto iter = m_selectedObjects.begin(); iter != m_selectedObjects.end(); ++iter)
            {
                if ((*iter)->GetID() == a_object->GetID())
                {
                    m_selectedObjects.erase(iter);

                    found = true;

                    break;
                }
            }

            if (!found)
            {
                m_selectedObjects.emplace_back(a_object);
            }
        }
        else
        {
            m_selectedObjects.clear();

            m_selectedObjects.emplace_back(a_object);
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
            Action* action = new CreateObjectAction(this, a_object);
            if (!PushAction(action))
            {
                printf("Error Creating Object \n");

                delete action;
            }
        }

        ImGui::Separator();

        if (ImGui::MenuItem("New Sphere(Curve)"))
        {
            Action* action = new CreateObjectAction(this, a_object, CreateObjectType_SphereCurve);
            if (!PushAction(action))
            {
                printf("Error Creating Object \n");

                delete action;
            }
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Delete Object"))
        {
            Action* action = new DeleteObjectAction(this, a_object);
            if (!PushAction(action))
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

void Workspace::Resize(int a_width, int a_height)
{
    m_reset = true;
}

void Workspace::Update(double a_delta)
{
    Application* app = Application::GetInstance();

    const int windowXPos = app->GetXPos();
    const int windowYPos = app->GetYPos();

    const int windowWidth = app->GetWidth();
    const int windowHeight = app->GetHeight();

    if (ImGui::BeginMainMenuBar())
    {
        m_barSize = ImGui::GetWindowSize().y;

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                New();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                Open(nullptr);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                Save();
            }

            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
            {
                SaveAs(nullptr);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                app->Close();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", nullptr, UndoEnabled()))
            {
                Undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", nullptr, RedoEnabled()))
            {
                Redo();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Preferences"))
            {

            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::BeginMenu("Layout"))
            {
                if (ImGui::MenuItem("Default"))
                {
                    m_init = true;
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("Editor"))
    {
        const ImVec2 pos = ImGui::GetWindowPos();
        const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        const ImVec2 size = { vMax.x - vMin.x, vMax.y - vMin.y };

        m_editor->Update(a_delta, { pos.x, pos.y }, { size.x, size.y });

        const RenderTexture* renderTexture = m_editor->GetRenderTexture();
        const Texture* texture = renderTexture->GetTexture();

        ImGui::Image((ImTextureID)texture->GetHandle(), size);

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("New Sphere(Curve)"))
            {
                Action* action = new CreateObjectAction(this, nullptr, CreateObjectType_SphereCurve);
                if (!PushAction(action))
                {
                    printf("Error Creating Object \n");
    
                    delete action;
                }
            }
            if (ImGui::MenuItem("New Cube(Curve)"))
            {
                Action* action = new CreateObjectAction(this, nullptr, CreateObjectType_CubeCurve);
                if (!PushAction(action))
                {
                    printf("Error Creating Object \n");

                    delete action;
                }
            }

            ImGui::EndPopup();
        }
    }
    ImGui::End();

    if (ImGui::Begin("Options"))
    {
        const e_EditorMode currentIndex = m_editor->GetEditorMode();

        if (ImGui::BeginCombo("##combo", EditorMode_String[m_editor->GetEditorMode()]))
        {
            for (int i = 0; i < EditorMode_End; ++i)
            {
                if (m_editor->IsEditorModeEnabled((e_EditorMode)i))
                {   
                    bool selected = currentIndex == i;
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
    }
    ImGui::End();

    if (ImGui::Begin("Toolbar"))
    {
        ImGui::BeginGroup();
        if (ImGui::Button("Move"))
        {

        }
        if (ImGui::Button("Rotate"))
        {

        }
        if (ImGui::Button("Scale"))
        {

        }
        ImGui::EndGroup();
    }
    ImGui::End();

    if (ImGui::Begin("Hierarchy"))
    {
        bool blk = false;

        for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
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
                Action* action = new CreateObjectAction(this, nullptr);
                if (!PushAction(action))
                {
                    printf("Error Creating Object \n");

                    delete action;
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("New Sphere(Curve)"))
            {
                Action* action = new CreateObjectAction(this, nullptr, CreateObjectType_SphereCurve);
                if (!PushAction(action))
                {
                    printf("Error Creating Object \n");

                    delete action;
                }
            }
            if (ImGui::MenuItem("New Cube(Curve)"))
            {
                Action* action = new CreateObjectAction(this, nullptr, CreateObjectType_CubeCurve);
                if (!PushAction(action))
                {
                    printf("Error Creating Object \n");

                    delete action;
                }
            }

            ImGui::EndPopup();
        }
    }
    ImGui::End();

    if (ImGui::Begin("Properties"))
    {
        const int size = m_selectedObjects.size();

        if (size > 0)
        {
            if (m_selectedObjects.size() == 1)
            {
                Object* obj = *m_selectedObjects.begin();

                char* buff = new char[1024];

                const char* name = obj->GetName();

                if (name == nullptr)
                {
                    name = "";
                }

                int size = strlen(name);

                for (int i = 0; i <= size; ++i)
                {
                    buff[i] = name[i];
                }

                if (ImGui::InputText("Name", buff, 1000))
                {
                    Action* action = new RenameObjectAction(name, buff, obj);
                    if (!PushAction(action))
                    {
                        printf("Error Renaming Object \n");

                        delete action;
                    }
                }

                CurveModel* model = obj->GetCurveModel();
                if (model != nullptr)
                {
                    int triSteps = model->GetSteps();

                    if (ImGui::InputInt("Model Resolution", &triSteps))
                    {
                        model->SetSteps(glm::max(triSteps, 0));
                        model->Triangulate();
                    }
                }
            }
            else
            {

            }
        }
    }
    ImGui::End();

    if (m_init)
    {
        m_init = false;

        DefaultWindowConfig();
    }
    if (m_reset)
    {
        m_reset = false;

        const ImGuiID id = ImGui::GetID("Dock Main");

        ImGui::DockBuilderSetNodePos(id, { windowXPos, windowYPos + m_barSize });
        ImGui::DockBuilderSetNodeSize(id, { windowWidth, windowHeight - m_barSize});
    }
}