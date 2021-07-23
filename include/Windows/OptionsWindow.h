#pragma once

#include "Windows/Window.h"

#include "Editor.h"

class Workspace;

class OptionsWindow : public Window
{
private:
    Workspace* m_workspace;
    Editor*    m_editor;

    void EditorFaceButton(const char* a_text, const char* a_path, e_EditorFaceCullingMode a_face, const char* a_tooltip);

protected:

public:
    OptionsWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~OptionsWindow();

    virtual void Update(double a_delta);
};