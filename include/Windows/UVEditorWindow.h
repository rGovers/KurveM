#pragma once

#include "Windows/Window.h"

class Editor;
class UVEditor;
class Workspace;

class UVEditorWindow : public Window
{
private:
    Workspace* m_workspace;
    Editor*    m_editor;
    UVEditor*  m_uvEditor;
    
protected:

public:
    UVEditorWindow(Workspace* a_workspace, Editor* a_editor, UVEditor* a_uvEditor);
    ~UVEditorWindow();

    virtual void Update(double a_delta);
};