#pragma once

#include "Windows/Window.h"

#include "Workspace.h"

class Editor;

class ToolbarWindow : public Window
{
private:
    Workspace* m_workspace;
    Editor*    m_editor;

    void ToolbarButton(const char* a_text, const char* a_path, e_ToolMode a_toolMode, const char* a_tooltip = "");
protected:

public:
    ToolbarWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~ToolbarWindow();

    virtual void Update(double a_delta);
};