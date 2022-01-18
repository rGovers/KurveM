#pragma once

#include "Windows/Window.h"

class Editor;
class ShapeEditor;
class Workspace;

class ShapeEditorWindow : public Window
{
private:
    Workspace* m_workspace;
    Editor*    m_editor;

    ShapeEditor*   m_shapeEditor;
protected:

public:
    ShapeEditorWindow(Workspace* a_workspace, Editor* a_editor, ShapeEditor* a_shapeEditor);
    ~ShapeEditorWindow();

    virtual void Update(double a_delta);
};