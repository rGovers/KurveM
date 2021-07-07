#include "Workspace.h"

#include <fstream>
#include <stdio.h>
#include <string>
#include <string.h>

#include "KurveMConfig.h"

#include "Actions/Action.h"
#include "Actions/CreateObjectAction.h"
#include "Actions/DeleteObjectAction.h"
#include "Actions/RenameObjectAction.h"
#include "Actions/ScaleObjectAction.h"
#include "Actions/TranslateObjectAction.h"
#include "Application.h"
#include "CurveModel.h"
#include "Datastore.h"
#include "Gizmos.h"
#include "ImGuiExt.h"
#include "imgui_internal.h"
#include "LongTasks/LongTask.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Modals/Modal.h"
#include "Modals/ExportOBJModal.h"
#include "Modals/SaveFileModal.h"
#include "Object.h"
#include "RenderTexture.h"
#include "Texture.h"
#include "Transform.h"

const char* EditorMode_String[] = { "Object Mode", "Edit Mode" };

void RunTasks(Workspace* a_workspace)
{
    int timeOut = 0;
    while (!a_workspace->IsShutingDown())
    {
        if (a_workspace->IsBlocked())
        {
            a_workspace->SetThreadClearState(true);
        }
        else
        {
            a_workspace->SetThreadClearState(false);

            LongTask* curTask = a_workspace->GetCurrentTask();

            if (curTask != nullptr && a_workspace->GetPostTask() == nullptr)
            {
                if (!curTask->Execute() || timeOut > 5)
                {
                    timeOut = 0;
                    a_workspace->PushCurrentTask();
                }
                else
                {
                    ++timeOut;
                }
            }
        }
    }

    a_workspace->PushJoinState();
}

void Workspace::PushCurrentTask()
{
    m_postTask = m_currentTask;
    m_currentTask = nullptr;
}

char* Workspace::GetHomePath() const
{
#if _WIN32
    const char* drive = getenv("HOMEDRIVE");
    const char* path = getenv("HOMEPATH");

    const int driveLen = strlen(drive);
    const int pathLen = strlen(path);

    const int len = driveLen + pathLen + 1;
                
    char* home = new char[len];

    for (int i = 0; i < driveLen; ++i)
    {
        home[i] = drive[i];
    }
    for (int i = 0; i < pathLen + 1; ++i)
    {
        home[i + driveLen] = path[i];
    }
#else
    const char* tmp = getenv("HOME");

    int len = strlen(tmp) + 1;

    char* home = new char[len];

    for (int i = 0; i < len; ++i)
    {
        home[i] = tmp[i];
    }
#endif

    return home;
}

Workspace::Workspace()
{   
    m_currentDir = nullptr;

    m_curAction = nullptr;

    m_init = true;
    m_reset = false;

    m_propertiesMode = ObjectPropertiesTab_Object;

    m_toolMode = ToolMode_Translate;

    m_shutDown = false;
    m_join = false;
    m_block = false;
    m_clear = false;

    m_currentTask = nullptr;
    m_postTask = nullptr;
    
    m_taskThread = std::thread(RunTasks, this);

    m_editor = new Editor(this);

    New();
}
Workspace::~Workspace()
{
    m_shutDown = true;

    while (!m_join)
    {
        std::this_thread::yield();   
    }
    
    m_taskThread.join();

    Gizmos::Destroy();
    Datastore::Destroy();

    ClearBuffers();
}

void Workspace::ClearBuffers()
{
    ClearSelectedObjects();

    for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
    {
        delete *iter;
    }
    m_objectList.clear();

    for (auto iter = m_actionQueue.begin(); iter != m_actionQueue.end(); ++iter)
    {
        delete *iter;
    }  
    m_actionQueue.clear();

    m_actionQueueIndex = m_actionQueue.end();

    if (m_postTask != nullptr)
    {
        delete m_postTask;
        m_postTask = nullptr;
    }

    for (auto iter = m_modalStack.begin(); iter != m_modalStack.end(); ++iter)
    {
        delete *iter;
    }
    m_modalStack.clear();

    for (auto iter = m_taskQueue.begin(); iter != m_taskQueue.end(); ++iter)
    {
        delete *iter;
    }
    m_taskQueue.clear();
}

void Workspace::New()
{
    m_block = true;

    while (!m_clear)
    {
        std::this_thread::yield();
    }

    ClearBuffers();

    Gizmos::Destroy();

    Datastore::Destroy();
    Datastore::Init();

    Gizmos::Init();

    m_block = false;
}
void Workspace::Open(const char* a_dir)
{
    New();
}

void SaveObject(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parentElement, const Object* a_object)
{
    if (a_object != nullptr)
    {
        tinyxml2::XMLElement* objectElement = a_doc->NewElement("Object");
        a_parentElement->InsertEndChild(objectElement);

        a_object->Serialize(a_doc, objectElement);

        const std::list<Object*> children = a_object->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            SaveObject(a_doc, objectElement, *iter);
        }
    }
}

void Workspace::Save()
{
    tinyxml2::XMLDocument doc;

    tinyxml2::XMLDeclaration* dec = doc.NewDeclaration();
    doc.InsertEndChild(dec);

    tinyxml2::XMLElement* scene = doc.NewElement("Scene");
    doc.InsertEndChild(scene);
    scene->SetAttribute("Major", KURVEM_VERSION_MAJOR);
    scene->SetAttribute("Minor", KURVEM_VERSION_MINOR);

    tinyxml2::XMLElement* objects = doc.NewElement("Objects");
    scene->InsertEndChild(objects);

    for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
    {
        SaveObject(&doc, objects, *iter);
    }

    doc.SaveFile(m_currentDir);
}
void Workspace::SaveAs(const char* a_dir)
{
    if (m_currentDir != nullptr)
    {
        delete[] m_currentDir;
        m_currentDir = nullptr;
    }

    if (a_dir != nullptr)
    {
        int len = strlen(a_dir) + 1;
        if (len > 1)
        {
            m_currentDir = new char[len];

            for (int i = 0; i < len; ++i)
            {
                m_currentDir[i] = a_dir[i];
            }
        }
    }

    if (m_currentDir != nullptr)
    {
        Save();
    }
}

void SaveOBJObject(std::ofstream* a_file, const Object* a_obj, bool a_smartStep, int a_steps)
{
    if (a_obj != nullptr)
    {
        a_file->write("\n", 1);

        a_obj->WriteOBJ(a_file, a_smartStep, a_steps);

        const std::list<Object*> children = a_obj->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            SaveOBJObject(a_file, *iter, a_smartStep, a_steps);
        }
    }    
}
void Workspace::ExportOBJ(const char* a_dir, bool a_selectedObjects, bool a_smartStep, int a_steps)
{
    std::ofstream file;

    file.open(a_dir); 
    if (file.is_open())
    {
        if (a_selectedObjects)
        {
            for (auto iter = m_selectedObjects.begin(); iter != m_selectedObjects.end(); ++iter)
            {
                (*iter)->WriteOBJ(&file, a_smartStep, a_steps);
            }
        }
        else
        {
            for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
            {
                SaveOBJObject(&file, *iter, a_smartStep, a_steps);
            }
        }

        file.close();
    }
}

bool Workspace::Undo()
{
    auto iter = m_actionQueueIndex;

    if (iter != m_actionQueue.begin())
    {
        Action* action = *--iter;

        if (action->Revert())
        {
            m_actionQueueIndex = iter;

            return true;
        }
    }

    return false;
}
bool Workspace::Redo()
{
    auto iter = m_actionQueueIndex;

    if (++iter != m_actionQueue.end())
    {
        Action* action = *iter;

        if (action->Redo())
        {
            m_actionQueueIndex = iter;

            return true;
        }
    }

    return false;
}

void Workspace::PushModal(Modal* a_modal)
{
    m_modalStack.emplace_back(a_modal);
}

bool Workspace::PushAction(Action* a_action)
{
    if (!a_action->Execute())
    {
        return false;
    }

    if (m_actionQueueIndex == m_actionQueue.end())
    {
        m_actionQueue.emplace_back(a_action);

        m_actionQueueIndex = m_actionQueue.end();

        return true;
    }

    while (true)
    {
        auto iter = m_actionQueueIndex;

        if (++iter == m_actionQueue.end())
        {
            break;
        }

        delete *iter;

        m_actionQueue.erase(iter);
    }

    m_actionQueue.emplace_back(a_action);

    m_actionQueueIndex = m_actionQueue.end();

    while (m_actionQueue.size() > 1000)
    {
        auto iter = m_actionQueue.begin();

        delete *iter;

        m_actionQueue.erase(iter);
    }

    return true;
}

void Workspace::PushLongTask(LongTask* a_longTask)
{
    if (a_longTask->PushAction(this))
    {
        m_taskQueue.emplace_back(a_longTask);
    }
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

void Workspace::ClearSelectedObjects()
{
    m_selectedObjects.clear();

    m_editor->ClearSelectedNodes();
}
void Workspace::AddSelectedObject(Object* a_object)
{
    for (auto iter = m_selectedObjects.begin(); iter != m_selectedObjects.end(); ++iter)
    {
        if ((*iter)->GetID() == a_object->GetID())
        {
            return;
        }
    }

    m_selectedObjects.emplace_back(a_object);

    m_editor->ClearSelectedNodes();
}
void Workspace::RemoveSelectedObject(Object* a_object)
{
    for (auto iter = m_selectedObjects.begin(); iter != m_selectedObjects.end(); ++iter)
    {
        if ((*iter)->GetID() == a_object->GetID())
        {
            m_selectedObjects.erase(iter);

            return;
        }
    }

    m_editor->ClearSelectedNodes();
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

    ImGui::DockBuilderSetNodePos(id, { (float)windowXPos, (float)windowYPos + m_barSize });
    ImGui::DockBuilderSetNodeSize(id, { (float)windowWidth, (float)windowHeight - m_barSize});

    const float topSideScale = 0.125f;
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

void Workspace::EditorFaceButton(const char* a_text, e_EditorFaceCullingMode a_face)
{
    if (ImGuiExt::ToggleButton(a_text, m_editor->GetEditorFaceCullingMode() == a_face, { 32, 32 }))
    {
        m_editor->SetEditorFaceCullingMode(a_face);
    }
}

void Workspace::ToolbarButton(const char* a_text, e_ToolMode a_toolMode)
{
    if (ImGuiExt::ToggleButton(a_text, m_toolMode == a_toolMode, { 32, 32 }))
    {
        m_toolMode = a_toolMode;
    }

    ImGui::NextColumn();
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
        m_curAction = nullptr;

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

        if (ImGui::MenuItem("New Cube(Curve)"))
            {
                Action* action = new CreateObjectAction(this, a_object, CreateObjectType_CubeCurve);
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

    if (m_currentTask == nullptr)
    {
        if (m_postTask != nullptr)
        {
            m_postTask->PostExecute();
            m_postTask = nullptr;

            delete m_postTask;
        }

        if (m_taskQueue.size() > 0)
        {
            LongTask* task = *m_taskQueue.begin();
            m_taskQueue.pop_front();
            
            m_currentTaskName = task->GetDisplayName();
            m_currentTask = task;
        }
    }

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

            if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, m_currentDir != nullptr))
            {
                Save();
            }

            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
            {
                char* home = GetHomePath();

                PushModal(new SaveFileModal(this, home));

                delete[] home;
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Export", m_objectList.size() > 0))
            {
                if (ImGui::MenuItem("Erde Curve Model"))
                {

                }

                ImGui::Separator();

                if (ImGui::MenuItem("Wavefront OBJ"))
                {
                    char* home = GetHomePath();

                    PushModal(new ExportOBJModal(this, home));

                    delete[] home;
                }

                if (ImGui::MenuItem("Collada"))
                {

                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Import"))
            {
                if (ImGui::MenuItem("Erde Curve Model"))
                {

                }

                ImGui::EndMenu();
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

        if (m_currentTask != nullptr)
        {
            ImGuiExt::Spinner("##spinner", 4, 1, ImGui::GetColorU32(ImGuiCol_Button));

            ImGui::SameLine();
            
            ImGui::Text(m_currentTaskName);
        }

        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("Options"))
    {
        const e_EditorMode currentIndex = m_editor->GetEditorMode();

        ImGui::PushItemWidth(128);
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

        ImGui::BeginGroup();

        EditorFaceButton("Front Faces", EditorFaceCullingMode_Back);
        ImGui::SameLine();
        EditorFaceButton("Back Faces", EditorFaceCullingMode_Front);
        ImGui::SameLine();
        EditorFaceButton("Both Faces", EditorFaceCullingMode_None);
        ImGui::SameLine();
        EditorFaceButton("No Faces", EditorFaceCullingMode_All);

        ImGui::EndGroup();
    }
    ImGui::End();

    if (ImGui::Begin("Toolbar"))
    {
        const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        const glm::vec2 size = { vMax.x - vMin.x, vMax.y - vMin.y };

        const int columns = glm::max(1, (int)glm::floor(size.x / 36.0f));

        ImGui::BeginGroup();
        ImGui::Columns(columns, nullptr, false);
        
        ToolbarButton("Translate", ToolMode_Translate);
        ToolbarButton("Rotate", ToolMode_Rotate);
        ToolbarButton("Scale", ToolMode_Scale);

        ImGui::Columns();
        ImGui::EndGroup();

        ImGui::Separator();

        ImGui::BeginGroup();
        ImGui::Columns(columns, nullptr, false);

        ToolbarButton("Extrude", ToolMode_Extrude);

        ImGui::Columns();
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
            Object* obj = *m_selectedObjects.begin();
            Transform* transform = obj->GetTransform();

            ImGui::BeginGroup();

            if (ImGui::Button("Object"))
            {
                m_propertiesMode = ObjectPropertiesTab_Object;
            }

            if (ImGui::Button("Curve"))
            {
                m_propertiesMode = ObjectPropertiesTab_Curve;
            }

            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();

            const unsigned int objectCount = m_selectedObjects.size();

            Object** objs = new Object*[objectCount];

            unsigned int index = 0;
            for (auto iter = m_selectedObjects.begin(); iter != m_selectedObjects.end(); ++iter)
            {
                objs[index++] = *iter;
            }

            switch (m_propertiesMode)
            {
            case ObjectPropertiesTab_Object:
            {
                if (m_selectedObjects.size() == 1)
                {
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
                        if (m_curAction != nullptr && m_curAction->GetActionType() == ActionType_RenameObject)
                        {
                            RenameObjectAction* action = (RenameObjectAction*)m_curAction;

                            action->SetNewName(buff);
                            action->Execute();
                        }
                        else
                        {
                            Action* action = new RenameObjectAction(name, buff, obj);

                            if (!PushAction(action))
                            {
                                printf("Error Renaming Object \n");

                                delete action;
                            }
                            else
                            {
                                m_curAction = action;
                            }
                        }
                    }
                }
                

                glm::vec3 pos = transform->Translation();
                if (ImGui::DragFloat3("Position", (float*)&pos, 0.1f))
                {
                    if (m_curAction != nullptr && m_curAction->GetActionType() == ActionType_TranslateObject)
                    {
                        TranslateObjectAction* action = (TranslateObjectAction*)m_curAction;

                        action->SetTranslation(pos);
                        action->Execute();
                    }
                    else
                    {
                        Action* action = new TranslateObjectAction(pos, objs, objectCount);
                        if (!PushAction(action))
                        {
                            printf("Error Renaming Object \n");

                            delete action;
                        }  
                        else
                        {
                            m_curAction = action;
                        }
                    }
                }

                glm::vec3 scale = transform->Scale();
                if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f))
                {
                    if (m_curAction != nullptr && m_curAction->GetActionType() == ActionType_ScaleObject)
                    {
                        ScaleObjectAction* action = (ScaleObjectAction*)m_curAction;

                        action->SetScale(scale);
                        action->Execute();
                    }
                    else
                    {
                        Action* action = new ScaleObjectAction(scale, objs, objectCount);
                        if (!PushAction(action))
                        {
                            printf("Error Renaming Object \n");

                            delete action;
                        }  
                        else
                        {
                            m_curAction = action;
                        }
                    }
                }

                break;
            }
            case ObjectPropertiesTab_Curve:
            {
                CurveModel* model = obj->GetCurveModel();
                if (model != nullptr)
                {
                    int triSteps = model->GetSteps();
                    if (ImGui::InputInt("Curve Resolution", &triSteps))
                    {
                        model->SetSteps(glm::max(triSteps, 1));
                        PushLongTask(new TriangulateCurveLongTask(model));
                    }

                    bool stepAdjust = model->IsStepAdjusted();
                    if (ImGui::Checkbox("Smart Step", &stepAdjust))
                    {
                        model->SetStepAdjust(stepAdjust);
                        PushLongTask(new TriangulateCurveLongTask(model));
                    }
                }

                break;
            }
            }

            ImGui::EndGroup();
        }
    }
    ImGui::End();

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

    if (m_modalStack.size() > 0)
    {
        Modal* modal = *--m_modalStack.end();

        if (!modal->Open())
        {
            delete modal;

            m_modalStack.pop_back();
        }
    }

    if (m_init)
    {
        m_init = false;

        DefaultWindowConfig();
    }
    if (m_reset)
    {
        m_reset = false;

        const ImGuiID id = ImGui::GetID("Dock Main");

        ImGui::DockBuilderSetNodePos(id, { (float)windowXPos, (float)windowYPos + m_barSize });
        ImGui::DockBuilderSetNodeSize(id, { (float)windowWidth, (float)windowHeight - m_barSize});
    }
}