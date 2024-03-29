#pragma once

#include <list>
#include <thread>

#include "Actions/Action.h"
#include "imgui.h"
#include "Object.h"

class Action;
class Animation;
class Editor;
class LongTask;
class Modal;
class ShapeEditor;
class UVEditor;
class Window;

enum e_DockMode
{
    DockMode_Default,
    DockMode_UVEditing
};
enum e_ToolMode
{
    ToolMode_Translate,
    ToolMode_Rotate,
    ToolMode_Scale,
    ToolMode_Extrude,
    ToolMode_End
};

class Workspace
{
private:
    char*                        m_currentDir;
                     
    ImGuiID                      m_centreDockNode;
                     
    std::list<Window*>           m_windows;

    Editor*                      m_editor;
    UVEditor*                    m_uvEditor;
    ShapeEditor*                 m_shapeEditor;
                     
    int                          m_barSize;

    e_DockMode                   m_dockMode;
    bool                         m_init;
    bool                         m_reset;

    e_ToolMode                   m_toolMode;

    // I am aware the queue exists I just find it easier to treat a list as a queue
    std::list<LongTask*>         m_taskQueue;
    LongTask*                    m_postTask;
    const char*                  m_currentTaskName;
    LongTask*                    m_currentTask;

    // This is not proper multithreading but serves the purpose of pushing tasks into the background 
    // and keeping the main thread responsive
    bool                         m_shutDown;
    bool                         m_join;
    bool                         m_block;
    bool                         m_clear;
    std::thread                  m_taskThread;

    Animation*                   m_currentAnimation;
    std::list<Animation*>        m_animations;

    std::list<Action*>           m_actionQueue;
    std::list<Action*>::iterator m_actionQueueIndex;
    
    std::list<Object*>           m_objectList;

    std::list<Modal*>            m_modalStack;

    std::list<Object*>           m_selectedObjects;

    Action*                      m_curAction;

    bool                         m_undoDown;
    bool                         m_redoDown;
    bool                         m_saveDown;
    bool                         m_editModeDown;

    inline bool UndoEnabled() const
    {
        return m_actionQueue.size() > 0 && m_actionQueueIndex != m_actionQueue.begin();
    }
    inline bool RedoEnabled() const
    {
        return m_actionQueue.size() > 0 && m_actionQueueIndex != m_actionQueue.end();
    }

    void ClearBuffers();

protected:

public:
    Workspace();
    ~Workspace();

    char* GetHomePath() const;
    
    void New();
    void Open(const char* a_dir);
    void Save() const;
    void SaveAs(const char* a_dir);

    void ExportOBJ(const char* a_dir, bool a_selectedObjects, bool a_smartStep, int a_steps, int a_pathSteps, int a_shapeSteps) const;
    void ExportCollada(const char* a_dir, bool a_exportAnimations, bool a_selectedObjects, bool a_smartStep, int a_steps, int a_pathSteps, int a_shapeSteps, const char* a_author, const char* a_copyright) const;

    bool Undo();
    bool Redo(); 

    void CreateCurveObjectMenuList(Object* a_parent);
    void CreatePathObjectMenuList(Object* a_parent);
    void ImportObjectMenuList(Object* a_parent);

    inline bool IsShutingDown() const
    {
        return m_shutDown;
    }
    inline bool IsBlocked() const
    {
        return m_block;
    }

    inline std::list<Animation*> GetAnimations() const
    {
        return m_animations;
    }
    void AddAnimation(Animation* a_animation);
    void RemoveAnimation(Animation* a_animation);

    inline Animation* GetCurrentAnimation() const
    {
        return m_currentAnimation;
    }
    inline void SetCurrentAnimation(Animation* a_animation)
    {
        m_currentAnimation = a_animation;
    }

    Object* GetObject(long long a_id) const;

    inline unsigned int GetSelectedObjectCount() const
    {
        return m_selectedObjects.size();
    }
    Object* GetSelectedObject() const;
    inline std::list<Object*> GetSelectedObjects() const
    {
        return m_selectedObjects;
    }
    Object** GetSelectedObjectArray() const;
    e_ObjectType GetSelectedObjectType() const;

    void ClearSelectedObjects();
    void AddSelectedObject(Object* a_object);
    void RemoveSelectedObject(Object* a_object);

    inline std::list<Object*> GetObjectList() const
    {
        return m_objectList;
    }
    std::list<Object*> GetAllObjectsList() const;

    inline e_ToolMode GetToolMode() const
    {
        return m_toolMode;
    }
    inline void SetToolMode(e_ToolMode a_toolMode)
    {
        m_toolMode = a_toolMode;
    }

    inline LongTask* GetCurrentTask() const
    {
        return m_currentTask;
    }
    inline LongTask* GetPostTask() const
    {
        return m_postTask;
    }

    inline void SetThreadClearState(bool a_value)
    {
        m_clear = a_value;
    }
    void PushCurrentTask();
    inline void PushJoinState()
    {
        m_join = true;
    }

    inline std::list<LongTask*> GetTaskQueue()
    {
        return m_taskQueue;
    }

    void PushModal(Modal* a_modal);

    bool PushAction(Action* a_action);
    void PushActionSet(Action* a_action, const char* a_errorMsg);
    void PushActionSet(Action* a_action, void* a_data, const char* a_errorMsg);

    inline void ClearCurrentAction() 
    {
        m_curAction = nullptr;
    }
    e_ActionType GetCurrentActionType() const;
    inline Action* GetCurrentAction() const
    {
        return m_curAction;
    }
    inline void SetCurrentAction(Action* a_action)
    {
        m_curAction = a_action;
    }

    void PushLongTask(LongTask* a_longTask);

    void AddObject(Object* a_object);
    void RemoveObject(Object* a_object);

    void ResetAnimationObjects();

    void DefaultWindowConfig();
    void UVWindowConfig();

    void Resize(int a_width, int a_height);

    void Update(double a_delta);
};