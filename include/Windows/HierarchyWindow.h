#pragma once

#include "Windows/Window.h"

class Editor;
class Object;
class Workspace;

class HierarchyWindow : public Window
{
private:
    Workspace* m_workspace;
    Editor*    m_editor;

    bool ObjectHeirachyGUI(Object* a_object, bool* a_blockMenu);

protected:

public:
    HierarchyWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~HierarchyWindow();

    virtual void Update(double a_delta);
};