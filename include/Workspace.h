#pragma once

#include <list>

#include "imgui.h"

class Action;
class Editor;
class Object;

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

    inline std::list<Object*> GetSelectedObjects() const
    {
        return m_selectedObjects;
    }
    inline std::list<Object*> GetObjectList() const
    {
        return m_objectList;
    }

    bool PushAction(Action* a_action);

    void AddObject(Object* a_object);
    void RemoveObject(Object* a_object);

    void ClearSelectedObjects();
    void AddSelectedObject(Object* a_object);
    void RemoveSelectedObject(Object* a_object);

    void DefaultWindowConfig();

    void Resize(int a_width, int a_height);

    void Update(double a_delta);

};