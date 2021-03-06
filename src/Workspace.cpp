#include "Workspace.h"

#include <fstream>
#include <stdio.h>
#include <string>
#include <string.h>

#include "KurveMConfig.h"

#include "Actions/Action.h"
#include "Actions/CreateObjectAction.h"
#include "Actions/DeleteObjectAction.h"
#include "Application.h"
#include "CurveModel.h"
#include "Datastore.h"
#include "EditorControls/ShapeEditor.h"
#include "EditorControls/UVEditor.h"
#include "Gizmos.h"
#include "IO/AnimationSerializer.h"
#include "IO/ObjectSerializer.h"
#include "ImGuiExt.h"
#include "imgui_internal.h"
#include "LongTasks/LongTask.h"
#include "Modals/ErrorModal.h"
#include "Modals/ExportColladaModal.h"
#include "Modals/ExportOBJModal.h"
#include "Modals/LoadFileModal.h"
#include "Modals/LoadReferenceImageModal.h"
#include "Modals/SaveFileModal.h"
#include "Model.h"
#include "Object.h"
#include "TransformVisualizer.h"
#include "Windows/AnimatorWindow.h"
#include "Windows/EditorWindow.h"
#include "Windows/HierarchyWindow.h"
#include "Windows/OptionsWindow.h"
#include "Windows/PropertiesWindow.h"
#include "Windows/ShapeEditorWindow.h"
#include "Windows/ToolbarWindow.h"
#include "Windows/UVEditorWindow.h"

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
                if (curTask->Execute() || timeOut > 5)
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

    const int len = strlen(tmp) + 1;

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
    Model::Init();

    m_currentDir = nullptr;

    m_curAction = nullptr;

    m_dockMode = DockMode_Default;
    m_init = true;
    m_reset = false;

    m_toolMode = ToolMode_Translate;

    m_shutDown = false;
    m_join = false;
    m_block = false;
    m_clear = false;

    m_currentTask = nullptr;
    m_postTask = nullptr;
    
    m_taskThread = std::thread(RunTasks, this);

    m_editor = nullptr;
    m_shapeEditor = nullptr;

    TransformVisualizer::Init();

    New();

    m_editor = new Editor(this);
    m_shapeEditor = new ShapeEditor(this, m_editor);
    m_uvEditor = new UVEditor(this, m_editor);

    m_windows.emplace_back(new AnimatorWindow(this, m_editor));
    m_windows.emplace_back(new EditorWindow(this, m_editor));
    m_windows.emplace_back(new HierarchyWindow(this, m_editor));
    m_windows.emplace_back(new OptionsWindow(this, m_editor));
    m_windows.emplace_back(new PropertiesWindow(this, m_editor));
    m_windows.emplace_back(new ShapeEditorWindow(this, m_editor, m_shapeEditor));
    m_windows.emplace_back(new ToolbarWindow(this, m_editor));
    m_windows.emplace_back(new UVEditorWindow(this, m_editor, m_uvEditor));
}
Workspace::~Workspace()
{
    TransformVisualizer::Destroy();

    for (auto iter = m_windows.begin(); iter != m_windows.end(); ++iter)
    {
        delete *iter;
    }

    m_shutDown = true;

    while (!m_join)
    {
        std::this_thread::yield();   
    }
    
    m_taskThread.join();

    Gizmos::Destroy();
    Datastore::Destroy();

    ClearBuffers();

    for (auto iter = m_modalStack.begin(); iter != m_modalStack.end(); ++iter)
    {
        delete *iter;
    }
    m_modalStack.clear();

    Model::Destroy();

    delete m_editor;
    delete m_shapeEditor;
    delete m_uvEditor;
}

void Workspace::ClearBuffers()
{
    m_currentAnimation = nullptr;

    ClearSelectedObjects();

    for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
    {
        delete *iter;
    }
    m_objectList.clear();

    for (auto iter = m_animations.begin(); iter != m_animations.end(); ++iter)
    {
        delete *iter;
    }
    m_animations.clear();

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

    for (auto iter = m_taskQueue.begin(); iter != m_taskQueue.end(); ++iter)
    {
        delete *iter;
    }
    m_taskQueue.clear();
}

void Workspace::New()
{
    m_block = true;

    if (m_currentDir != nullptr)
    {
        delete[] m_currentDir;
        m_currentDir = nullptr;
    }

    while (!m_clear)
    {
        std::this_thread::yield();
    }

    ClearBuffers();

    Gizmos::Destroy();

    Datastore::Destroy();
    Datastore::Init();

    Gizmos::Init();

    if (m_editor != nullptr)
    {
        m_editor->Init();
    }
    if (m_shapeEditor != nullptr)
    {
        m_shapeEditor->Init();
    }

    m_block = false;

    printf("Initialized Workspace \n");
}
void Workspace::Open(const char* a_dir)
{
    printf("Attempting to Open: ");
    printf(a_dir);
    printf("\n");

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(a_dir) == tinyxml2::XMLError::XML_SUCCESS)
    {
        New();

        const int len = strlen(a_dir) + 1;

        m_currentDir = new char[len];

        for (int i = 0; i < len; ++i)
        {
            m_currentDir[i] = a_dir[i];
        }

        const tinyxml2::XMLElement* sceneElement = doc.FirstChildElement("Scene");
        if (sceneElement != nullptr)
        {
            std::list<ObjectBoneGroup> bones;
            std::unordered_map<long long, long long> idMap;

            for (const tinyxml2::XMLElement* iter = sceneElement->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
            {
                const char* str = iter->Value();

                if (strcmp(str, "Objects") == 0)
                {
                    for (const tinyxml2::XMLElement* innerIter = iter->FirstChildElement(); innerIter != nullptr; innerIter = innerIter->NextSiblingElement())
                    {
                        m_objectList.emplace_back(ObjectSerializer::ParseData(this, m_editor, innerIter, nullptr, &bones, &idMap));
                    }
                }
                else if (strcmp(str, "Animations") == 0)
                {
                    for (const tinyxml2::XMLElement* innerIter = iter->FirstChildElement(); innerIter != nullptr; innerIter = innerIter->NextSiblingElement())
                    {
                        const char* str = innerIter->Value();
                        if (strcmp("Animation", str) == 0)
                        {
                            Animation* anim = AnimationSerializer::ParseData(innerIter);
                            if (anim != nullptr)
                            {
                                m_animations.emplace_back(anim);
                            }
                        }
                        else
                        {
                            printf("Error Opening File: Invalid animation node: ");
                            printf(str);
                            printf("\n");
                        }
                    }
                }
                else
                {
                    PushModal(new ErrorModal("Error Opening File: Invalid scene child"));

                    printf("Invalid scene child: ");
                    printf(str);
                    printf("\n");
                }
            }

            for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
            {
                ObjectSerializer::PostParseData(*iter, bones, idMap);
            }

            for (auto iter = m_animations.begin(); iter != m_animations.end(); ++iter)
            {
                AnimationSerializer::PostParseData(*iter, idMap);
            }
        }
        else
        {
            PushModal(new ErrorModal("Error Opening File: No Scene Node"));
        }
    }
    else
    {
        PushModal(new ErrorModal("Cannot Open File"));
    }
}

void SaveObject(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parentElement, const Object* a_object)
{
    if (a_object != nullptr)
    {
        tinyxml2::XMLElement* objectElement;
        switch (a_object->GetObjectType())
        {
        case ObjectType_ArmatureNode:
        {
            objectElement = a_doc->NewElement("ArmatureNode");

            break;
        }
        default:
        {
            objectElement = a_doc->NewElement("Object");

            break;
        }
        }
        a_parentElement->InsertEndChild(objectElement);

        ObjectSerializer::Serialize(a_doc, objectElement, a_object);

        const std::list<Object*> children = a_object->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            SaveObject(a_doc, objectElement, *iter);
        }
    }
}

void Workspace::Save() const
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

    tinyxml2::XMLElement* animations = doc.NewElement("Animations");
    scene->InsertEndChild(animations);

    for (auto iter = m_animations.begin(); iter != m_animations.end(); ++iter)
    {
        AnimationSerializer::Serialize(&doc, animations, *iter);
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
        const int len = strlen(a_dir) + 1;
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

void SaveOBJObject(std::ofstream* a_file, const Object* a_obj, bool a_smartStep, int a_steps, int a_pathSteps, int a_shapeSteps)
{
    if (a_obj != nullptr)
    {
        a_file->write("\n", 1);

        ObjectSerializer::WriteObj(a_file, a_obj, a_smartStep, a_steps, a_pathSteps, a_shapeSteps);

        const std::list<Object*> children = a_obj->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            SaveOBJObject(a_file, *iter, a_smartStep, a_steps, a_pathSteps, a_shapeSteps);
        }
    }    
}
void Workspace::ExportOBJ(const char* a_dir, bool a_selectedObjects, bool a_smartStep, int a_steps, int a_pathSteps, int a_shapeSteps) const
{
    std::ofstream file;

    file.open(a_dir); 
    if (file.is_open())
    {
        if (a_selectedObjects)
        {
            for (auto iter = m_selectedObjects.begin(); iter != m_selectedObjects.end(); ++iter)
            {
                ObjectSerializer::WriteObj(&file, *iter, a_smartStep, a_steps, a_pathSteps, a_shapeSteps);
            }
        }
        else
        {
            for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
            {
                SaveOBJObject(&file, *iter, a_smartStep, a_steps, a_pathSteps, a_shapeSteps);
            }
        }

        file.close();
    }
}

void SaveColladaObject(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_geometryElement, tinyxml2::XMLElement* a_controllerElement, tinyxml2::XMLElement* a_parent, const Object* a_obj, bool a_stepAdjust, int a_steps, int a_pathSteps, int a_shapeSteps)
{
    if (a_obj != nullptr)
    {
        tinyxml2::XMLElement* pElement = ObjectSerializer::WriteCollada(a_doc, a_geometryElement, a_controllerElement, a_parent, a_obj, a_stepAdjust, a_steps, a_pathSteps, a_shapeSteps);

        const std::list<Object*> children = a_obj->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            if (pElement != nullptr)
            {
                SaveColladaObject(a_doc, a_geometryElement, a_controllerElement, pElement, *iter, a_stepAdjust, a_steps, a_pathSteps, a_shapeSteps);
            }
            else
            {
                SaveColladaObject(a_doc, a_geometryElement, a_controllerElement, a_parent, *iter, a_stepAdjust, a_steps, a_pathSteps, a_shapeSteps);
            }
        }
    }
}
void Workspace::ExportCollada(const char* a_dir, bool a_exportAnimations, bool a_selectedObjects, bool a_smartStep, int a_steps, int a_pathSteps, int a_shapeSteps, const char* a_author, const char* a_copyright) const
{
    tinyxml2::XMLDocument doc;

    tinyxml2::XMLDeclaration* dec = doc.NewDeclaration();
    doc.InsertEndChild(dec);

    tinyxml2::XMLElement* rootElement = doc.NewElement("COLLADA");
    doc.InsertEndChild(rootElement);
    rootElement->SetAttribute("xmlns", "http://www.collada.org/2008/03/COLLADASchema");
    rootElement->SetAttribute("version", "1.5.0");

    tinyxml2::XMLElement* assetElement = doc.NewElement("asset");
    rootElement->InsertEndChild(assetElement);

    tinyxml2::XMLElement* contributorElement = doc.NewElement("contributor");
    assetElement->InsertEndChild(contributorElement);

    tinyxml2::XMLElement* authorElement = doc.NewElement("author");
    contributorElement->InsertEndChild(authorElement);
    if (a_author != nullptr)
    {
        authorElement->SetText(a_author);
    }

    tinyxml2::XMLElement* authoringToolElement = doc.NewElement("authoring_tool");
    contributorElement->InsertEndChild(authoringToolElement);
    const std::string toolName = "KurveM " + std::to_string(KURVEM_VERSION_MAJOR) + "." + std::to_string(KURVEM_VERSION_MINOR);
    authoringToolElement->SetText(toolName.c_str());

    tinyxml2::XMLElement* commentsElement = doc.NewElement("comments");
    contributorElement->InsertEndChild(commentsElement);
    commentsElement->SetText("Export from KurveM. Converted from curves and paths to polygon mesh.");

    tinyxml2::XMLElement* copyrightElement = doc.NewElement("copyright");
    contributorElement->InsertEndChild(copyrightElement);
    if (a_copyright != nullptr)
    {
        copyrightElement->SetText(a_copyright);
    }

    // tinyxml2::XMLElement* sourceDataElement = doc.NewElement("source_data");
    // contributorElement->InsertEndChild(sourceDataElement);

    tinyxml2::XMLElement* createdElement = doc.NewElement("created");
    assetElement->InsertEndChild(createdElement);

    tinyxml2::XMLElement* modifiedElement = doc.NewElement("modified");
    assetElement->InsertEndChild(modifiedElement);

    tinyxml2::XMLElement* unitElement = doc.NewElement("unit");
    assetElement->InsertEndChild(unitElement);
    unitElement->SetAttribute("meter", 1);
    unitElement->SetAttribute("name", "meter");

    tinyxml2::XMLElement* upAxisElement = doc.NewElement("up_axis");
    assetElement->InsertEndChild(upAxisElement);
    upAxisElement->SetText("Y_UP");

    tinyxml2::XMLElement* libraryGeometriesElement = doc.NewElement("library_geometries");
    rootElement->InsertEndChild(libraryGeometriesElement);

    tinyxml2::XMLElement* libraryContollersElement = doc.NewElement("library_controllers");
    rootElement->InsertEndChild(libraryContollersElement);

    tinyxml2::XMLElement* libraryVisualSceneElement = doc.NewElement("library_visual_scenes");
    rootElement->InsertEndChild(libraryVisualSceneElement);

    tinyxml2::XMLElement* sceneElement = doc.NewElement("visual_scene");
    libraryVisualSceneElement->InsertEndChild(sceneElement);
    sceneElement->SetAttribute("id", "DefaultScene");

    if (a_selectedObjects)
    {
        for (auto iter = m_selectedObjects.begin(); iter != m_selectedObjects.end(); ++iter)
        {
            ObjectSerializer::WriteCollada(&doc, libraryGeometriesElement, libraryContollersElement, sceneElement, *iter, a_smartStep, a_steps, a_pathSteps, a_shapeSteps);
        }
    }
    else
    {
        for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
        {
            SaveColladaObject(&doc, libraryGeometriesElement, libraryContollersElement, sceneElement, *iter, a_smartStep, a_steps, a_pathSteps, a_shapeSteps);
        }
    }

    if (a_exportAnimations && m_animations.size() > 0)
    {
        tinyxml2::XMLElement* libraryAnimationsElement = doc.NewElement("library_animations");
        rootElement->InsertEndChild(libraryAnimationsElement);

        for (auto iter = m_animations.begin(); iter != m_animations.end(); ++iter)
        {
            AnimationSerializer::WriteCollada(this, &doc, libraryAnimationsElement, *iter);
        }
    }

    doc.SaveFile(a_dir);
}

bool Workspace::Undo()
{
    m_curAction = nullptr;

    auto iter = m_actionQueueIndex;

    if (iter != m_actionQueue.begin())
    {
        Action* action = *--iter;

        if (action->Revert())
        {
            --m_actionQueueIndex;

            return true;
        }
    }

    return false;
}
bool Workspace::Redo()
{
    m_curAction = nullptr;

    auto iter = m_actionQueueIndex;

    if (iter != m_actionQueue.end())
    {
        Action* action = *iter;

        if (action->Redo())
        {
            ++m_actionQueueIndex;

            return true;
        }
    }

    return false;
}

void Workspace::AddAnimation(Animation* a_animation)
{
    for (auto iter = m_animations.begin(); iter != m_animations.end(); ++iter)
    {
        if (*iter == a_animation)
        {
            return;
        }
    }

    m_animations.emplace_back(a_animation);
}
void Workspace::RemoveAnimation(Animation* a_animation)
{
    if (m_currentAnimation == a_animation)
    {
        m_currentAnimation = nullptr;
    }

    for (auto iter = m_animations.begin(); iter != m_animations.end(); ++iter)
    {
        if (*iter == a_animation)
        {
            m_animations.erase(iter);

            return;
        }
    }
}

Object* GetObjects(Object* a_object, long long a_id)
{
    if (a_object->GetID() == a_id)
    {
        return a_object;
    }

    const std::list<Object*> children = a_object->GetChildren();

    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        Object* obj = GetObjects(*iter, a_id);

        if (obj != nullptr)
        {
            return obj;
        }
    }

    return nullptr;
}

Object* Workspace::GetObject(long long a_id) const
{
    for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
    {
        Object* obj = GetObjects(*iter, a_id);

        if (obj != nullptr)
        {
            return obj;
        }
    }

    return nullptr;
}

Object* Workspace::GetSelectedObject() const
{
    if (m_selectedObjects.size() > 0)
    {
        return *m_selectedObjects.begin();
    }

    return nullptr;
}
Object** Workspace::GetSelectedObjectArray() const
{
    const unsigned int objectCount = m_selectedObjects.size();

    if (objectCount > 0)
    {
        Object** objs = new Object*[objectCount];

        int index = 0;
        for (auto iter = m_selectedObjects.begin(); iter != m_selectedObjects.end(); ++iter)
        {
            objs[index++] = *iter;
        }

        return objs;
    }
    
    return nullptr;
}
e_ObjectType Workspace::GetSelectedObjectType() const
{
    if (m_selectedObjects.size() > 0)
    {
        return (*m_selectedObjects.begin())->GetObjectType();
    }

    return ObjectType_Empty;
}

void Workspace::PushModal(Modal* a_modal)
{
    for (auto iter = m_modalStack.begin(); iter != m_modalStack.end(); ++iter)
    {
        if (strcmp((*iter)->GetName(), a_modal->GetName()) == 0)
        {
            delete a_modal;

            return;
        }
    }

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
void Workspace::PushActionSet(Action* a_action, const char* a_errorMsg)
{
    if (PushAction(a_action))
    {
        m_curAction = a_action;
    }
    else
    {
        delete a_action;
    
        printf(a_errorMsg);
        printf("\n");
    }
}
void Workspace::PushActionSet(Action* a_action, void* a_data, const char* a_errorMsg)
{
    if (m_curAction != nullptr && m_curAction->GetActionType() == a_action->GetActionType())
    {
        delete a_action;

        m_curAction->SetData(a_data);
        m_curAction->Execute();
    }
    else
    {
        PushActionSet(a_action, a_errorMsg);
    }
}

e_ActionType Workspace::GetCurrentActionType() const
{
    if (m_curAction != nullptr)
    {
        return m_curAction->GetActionType();
    }

    return ActionType_Null;
}

void Workspace::PushLongTask(LongTask* a_longTask)
{
    if (a_longTask->PushAction(this))
    {
        m_taskQueue.emplace_back(a_longTask);
    }
    else
    {
        delete a_longTask;
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

void ResetAnimationObject(Object* a_parent)
{
    if (a_parent != nullptr)
    {
        a_parent->ResetAnimation();

        const std::list<Object*> children = a_parent->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            ResetAnimationObject(*iter);
        }
    }
}
void Workspace::ResetAnimationObjects()
{
    for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
    {
        ResetAnimationObject(*iter);
    }
}

void Workspace::ClearSelectedObjects()
{
    m_selectedObjects.clear();

    if (m_editor != nullptr)
    {
        m_editor->ClearSelectedNodes();
        m_shapeEditor->ClearSelectedNodes();
    }

    m_curAction = nullptr;
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
    m_shapeEditor->ClearSelectedNodes();

    m_curAction = nullptr;
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
    m_shapeEditor->ClearSelectedNodes();

    m_curAction = nullptr;
}

void GetAllObjects(Object* a_object, std::list<Object*>* a_objs)
{
    if (a_object != nullptr)
    {
        a_objs->emplace_back(a_object);
    }

    const std::list<Object*> children = a_object->GetChildren();

    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        GetAllObjects(*iter, a_objs);
    }
}

std::list<Object*> Workspace::GetAllObjectsList() const
{
    std::list<Object*> objs;

    for (auto iter = m_objectList.begin(); iter != m_objectList.end(); ++iter)
    {
        GetAllObjects(*iter, &objs);
    }

    return objs;
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
    ImGuiID dockRightTop = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, rightSideScale, nullptr, &dockMainID);
    const ImGuiID dockRightBottom = ImGui::DockBuilderSplitNode(dockRightTop, ImGuiDir_Down, 0.25f, nullptr, &dockRightTop);
    const ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.2f, nullptr, &dockMainID);
    const ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Up, topSideScale, nullptr, &dockMainID);
    const ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.5f, nullptr, &dockMainID);
    const ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, leftSideScale, nullptr, &dockMainID);

    ImGui::DockBuilderDockWindow("Options", dockTop);

    ImGui::DockBuilderDockWindow("Editor", dockMainID);
    ImGui::DockBuilderDockWindow("UV Editor", dockMainID);
    ImGui::DockBuilderDockWindow("Shape Editor", dockRight);

    ImGui::DockBuilderDockWindow("Hierarchy", dockRightTop);
    ImGui::DockBuilderDockWindow("Properties", dockRightBottom);

    ImGui::DockBuilderDockWindow("Toolbar", dockLeft);

    ImGui::DockBuilderDockWindow("Animator", dockBottom);

    ImGui::DockBuilderFinish(id);
}
void Workspace::UVWindowConfig()
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
    const float rightSideScale = 0.5f;

    ImGuiID dockMainID = id;
    const ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Up, topSideScale, nullptr, &dockMainID);
    const ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.2f, nullptr, &dockMainID);
    const ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, rightSideScale, nullptr, &dockMainID);
    const ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, leftSideScale, nullptr, &dockMainID);
    const ImGuiID dockMidBottom = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.5f, nullptr, &dockMainID);

    ImGui::DockBuilderDockWindow("Options", dockTop);

    ImGui::DockBuilderDockWindow("Editor", dockMainID);
    ImGui::DockBuilderDockWindow("UV Editor", dockRight);
    ImGui::DockBuilderDockWindow("Shape Editor", dockMidBottom);

    ImGui::DockBuilderDockWindow("Toolbar", dockLeft);
    ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
    ImGui::DockBuilderDockWindow("Properties", dockLeft);

    ImGui::DockBuilderDockWindow("Animator", dockBottom);

    ImGui::DockBuilderFinish(id);
}

void Workspace::CreateCurveObjectMenuList(Object* a_parent)
{
    if (ImGuiExt::Image("Textures/OBJECT_CURVE.png", glm::vec2(16.0f)))
    {
        ImGui::SameLine();
    }

    if (ImGui::BeginMenu("New Curve Object"))
    {
        if (ImGui::MenuItem("Triangle"))
        {
            Action* action = new CreateObjectAction(this, a_parent, CreateObjectType_TriangleCurve);
            if (!PushAction(action))
            {
                printf("Error Creating Curve Object(Triangle) \n");

                delete action;
            }
        }

        if (ImGui::MenuItem("Plane"))
        {
            Action* action = new CreateObjectAction(this, a_parent, CreateObjectType_PlaneCurve);
            if (!PushAction(action))
            {
                printf("Error Creating Curve Object(Plane) \n");

                delete action;
            }
        }

        if (ImGui::MenuItem("Sphere"))
        {
            Action* action = new CreateObjectAction(this, a_parent, CreateObjectType_SphereCurve);
            if (!PushAction(action))
            {
                printf("Error Creating Curve Object(Sphere) \n");

                delete action;
            }
        }

        if (ImGui::MenuItem("Cube"))
        {
            Action* action = new CreateObjectAction(this, a_parent, CreateObjectType_CubeCurve);
            if (!PushAction(action))
            {
                printf("Error Creating Curve Object(Cube) \n");

                delete action;
            }
        }

        ImGui::EndMenu();
    }
}
void Workspace::CreatePathObjectMenuList(Object* a_parent)
{
    if (ImGuiExt::Image("Textures/OBJECT_PATH.png", glm::vec2(16.0f)))
    {
        ImGui::SameLine();
    }

    if (ImGui::BeginMenu("New Path Object"))
    {
        if (ImGui::MenuItem("Cylinder"))
        {
            Action* action = new CreateObjectAction(this, a_parent, CreateObjectType_CylinderPath);
            if (!PushAction(action))
            {
                printf("Error Creating Path Object(Cylinder) \n");

                delete action;
            }
        }

        if (ImGui::MenuItem("Cone"))
        {
            Action* action = new CreateObjectAction(this, a_parent, CreateObjectType_ConePath);
            if (!PushAction(action))
            {
                printf("Error Creating Path Object(Cone) \n");

                delete action;
            }
        }

        if (ImGui::MenuItem("Spiral"))
        {
            Action* action = new CreateObjectAction(this, a_parent, CreateObjectType_SpiralPath);
            if (!PushAction(action))
            {
                printf("Error Creating Path Object(Spiral) \n");

                delete action;
            }
        }

        if (ImGui::MenuItem("Torus"))
        {
            Action* action = new CreateObjectAction(this, a_parent, CreateObjectType_TorusPath);
            if (!PushAction(action))
            {
                printf("Error Creating Path Object(Torus) \n");

                delete action;
            }
        }

        ImGui::EndMenu();
    }
}
void Workspace::ImportObjectMenuList(Object* a_parent)
{
    if (ImGuiExt::Image("Textures/OBJECT_REFERENCEIMAGE.png", glm::vec2(16.0f)))
    {
        ImGui::SameLine();
    }

    if (ImGui::BeginMenu("New Reference Image"))
    {
        char* home = GetHomePath();

        if (ImGui::MenuItem("PNG"))
        {
            PushModal(new LoadReferenceImageModal(this, a_parent, home, ".png"));
        }
        if (ImGui::MenuItem("BMP"))
        {
            PushModal(new LoadReferenceImageModal(this, a_parent, home, ".bmp"));
        }
        if (ImGui::MenuItem("JPEG"))
        {
            PushModal(new LoadReferenceImageModal(this, a_parent, home, ".jpg"));
        }

        delete[] home;

        ImGui::EndMenu();
    }
}

void Workspace::Resize(int a_width, int a_height)
{
    m_reset = true;
}

void Workspace::Update(double a_delta)
{
    Application* app = Application::GetInstance();
    GLFWwindow* window = app->GetWindow();

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
    {
        if (!m_undoDown && glfwGetKey(window, GLFW_KEY_Z))
        {
            m_undoDown = true;

            Undo();
        }
        else if (!glfwGetKey(window, GLFW_KEY_Z))
        {
            m_undoDown = false;
        }

        if (!m_redoDown && glfwGetKey(window, GLFW_KEY_Y))
        {
            m_redoDown = true;

            Redo();
        }
        else if (!glfwGetKey(window, GLFW_KEY_Y))
        {
            m_redoDown = false;
        }

        if (glfwGetKey(window, GLFW_KEY_N))
        {
            New();
        }

        if (glfwGetKey(window, GLFW_KEY_O))
        {
            char* home = GetHomePath();

            PushModal(new LoadFileModal(this, home));

            delete[] home;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
        {
            if (glfwGetKey(window, GLFW_KEY_S))
            {
                char* home = GetHomePath();

                PushModal(new SaveFileModal(this, home));

                delete[] home;
            }
        }
        else
        {
            if (!m_shutDown && glfwGetKey(window, GLFW_KEY_S))
            {
                m_saveDown = true;

                Save();
            }
            else if (!glfwGetKey(window, GLFW_KEY_S))
            {
                m_saveDown = false;
            }
        }
    }
    else 
    {
        if (glfwGetKey(window, GLFW_KEY_T))
        {
            m_toolMode = ToolMode_Translate;
        }
        if (glfwGetKey(window, GLFW_KEY_R))
        {
            m_toolMode = ToolMode_Rotate;
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
            {
                m_toolMode = ToolMode_Scale;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_E))
        {
            m_toolMode = ToolMode_Extrude;
        }

        if (!m_editModeDown && glfwGetKey(window, GLFW_KEY_TAB))
        {
            m_editModeDown = true;

            if (m_editor->IsEditorModeEnabled(EditorMode_Edit))
            {
                if (m_editor->GetEditorMode() == EditorMode_Edit)
                {
                    m_editor->SetEditorMode(EditorMode_Object);
                }
                else
                {
                    m_editor->SetEditorMode(EditorMode_Edit);
                }
            }   
        }
        else if (!glfwGetKey(window, GLFW_KEY_TAB))
        {
            m_editModeDown = false;
        }
    }

    if (m_currentTask == nullptr)
    {
        if (m_postTask != nullptr)
        {
            m_postTask->PostExecute();
            
            delete m_postTask;
            m_postTask = nullptr;
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
                char* home = GetHomePath();

                PushModal(new LoadFileModal(this, home));

                delete[] home;
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
                if (ImGui::MenuItem("Erde Curve Model(.ECMdl)"))
                {

                }

                ImGui::Separator();

                if (ImGui::MenuItem("Wavefront OBJ(.obj)"))
                {
                    char* home = GetHomePath();

                    PushModal(new ExportOBJModal(this, home));

                    delete[] home;
                }

                if (ImGui::MenuItem("Collada(.dae)"))
                {
                    char* home = GetHomePath();

                    PushModal(new ExportColladaModal(this, home));

                    delete[] home;
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Import"))
            {
                if (ImGui::MenuItem("Erde Curve Model(.ECMdl)"))
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
                    m_dockMode = DockMode_Default;
                    m_init = true;
                }
                
                if (ImGui::MenuItem("UV Editing"))
                {
                    m_dockMode = DockMode_UVEditing;
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

    for (auto iter = m_windows.begin(); iter != m_windows.end(); ++iter)
    {
        (*iter)->Update(a_delta);
    }

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

        switch (m_dockMode)
        {
        case DockMode_UVEditing:
        {
            UVWindowConfig();

            break;
        }
        default:
        {   
            DefaultWindowConfig();

            break;
        }
        }
    }
    if (m_reset)
    {
        m_reset = false;

        const ImGuiID id = ImGui::GetID("Dock Main");

        ImGui::DockBuilderSetNodePos(id, { (float)windowXPos, (float)windowYPos + m_barSize });
        ImGui::DockBuilderSetNodeSize(id, { (float)windowWidth, (float)windowHeight - m_barSize});
    }
}