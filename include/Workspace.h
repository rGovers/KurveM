#pragma once

#include <list>
#include <thread>

#include "imgui.h"

class Action;
class Editor;
class LongTask;
class Object;

enum e_ToolMode
{
    ToolMode_Translate,
    ToolMode_Rotate,
    ToolMode_Scale,
    ToolMode_Extrude,
    ToolMode_End
};

enum e_ObjectPropertiesTab
{
    ObjectPropertiesTab_Object,
    ObjectPropertiesTab_Curve,
    ObjectPropertiesTab_End,
};

class Workspace
{
private:
    char*                        m_currentDir;
                     
    ImGuiID                      m_centreDockNode;
                     
    Editor*                      m_editor;
                     
    int                          m_barSize;

    bool                         m_init;
    bool                         m_reset;

    e_ObjectPropertiesTab        m_propertiesMode;

    e_ToolMode                   m_toolMode;

    // I am aware the queue exists I just find it easier to treat a list as a queue
    std::list<LongTask*>         m_taskQueue;
    LongTask*                    m_postTask;
    const char*                  m_currentTaskName;
    LongTask*                    m_currentTask;

    // This is not proper multithreading but serves the purpose of pushing tasks into the background 
    // and keeping the main thread responsive
    bool                         m_shutdown;
    bool                         m_join;
    std::thread                  m_taskThread;

    std::list<Action*>           m_actionStack;
    std::list<Action*>::iterator m_actionStackIndex;
    std::list<Object*>           m_objectList;

    std::list<Object*>           m_selectedObjects;

    Action*                      m_curAction;

    inline bool UndoEnabled() const
    {
        return m_actionStack.size() > 0 && m_actionStackIndex != m_actionStack.begin();
    }
    inline bool RedoEnabled() const
    {
        return m_actionStack.size() > 0 && m_actionStackIndex != m_actionStack.end();
    }

    void ClearBuffers();

    void ToolbarButton(const char* a_text, e_ToolMode a_toolMode);
    bool ObjectHeirachyGUI(Object* a_object, bool* a_blockMenu);

protected:

public:
    Workspace();
    ~Workspace();

    void New();
    void Open(const char* a_dir);
    void Save();
    void SaveAs(const char* a_dir);

    bool Undo();
    bool Redo();

    bool IsShutingDown() const
    {
        return m_shutdown;
    }

    inline Object* GetSelectedObject() const
    {
        return *m_selectedObjects.begin();
    }
    inline std::list<Object*> GetSelectedObjects() const
    {
        return m_selectedObjects;
    }
    inline std::list<Object*> GetObjectList() const
    {
        return m_objectList;
    }

    inline e_ToolMode GetToolMode() const
    {
        return m_toolMode;
    }

    inline LongTask* GetCurrentTask() const
    {
        return m_currentTask;
    }
    inline LongTask* GetPostTask() const
    {
        return m_postTask;
    }

    void PushCurrentTask();
    void PushJoinState();

    inline std::list<LongTask*> GetTaskQueue()
    {
        return m_taskQueue;
    }

    bool PushAction(Action* a_action);

    void PushLongTask(LongTask* a_longTask);

    void AddObject(Object* a_object);
    void RemoveObject(Object* a_object);

    void ClearSelectedObjects();
    void AddSelectedObject(Object* a_object);
    void RemoveSelectedObject(Object* a_object);

    void DefaultWindowConfig();

    void Resize(int a_width, int a_height);

    void Update(double a_delta);
};