#pragma once

#include "Windows/Window.h"

class Editor;
class Workspace;

class AnimatorWindow : public Window
{
private:
    Workspace* m_workspace;
    Editor*    m_editor;

    bool       m_playing;

protected:

public:
    AnimatorWindow(Workspace* a_workspace, Editor* a_editor);
    ~AnimatorWindow();

    virtual void Update(double a_delta);
};