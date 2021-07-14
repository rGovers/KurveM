#pragma once

#include "Windows/Window.h"

class Editor;
class Workspace;

class EditorWindow : public Window
{
private:
    Workspace* m_workspace;
    Editor*    m_editor;

protected:

public:
    EditorWindow(Workspace* a_workspace, Editor* a_editor);
    ~EditorWindow();

    virtual void Update(double a_delta);
};