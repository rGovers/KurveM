#pragma once

#include <functional>

#include "Application.h"

class Editor;
class Object;
class Workspace;

class EditorInputController
{
private:
    Workspace* m_workspace;
    Editor*    m_editor;

    bool       m_deleteDown;
    bool       m_fDown;
    bool       m_nDown;

    void KeyDown(GLFWwindow* a_window, int a_key, bool* a_state, Object* a_obj, const std::function<void(Object*)>& a_curveFunction) const;

protected:

public:
    EditorInputController(Workspace* a_workspace, Editor* a_editor);
    ~EditorInputController();

    void Update(double a_delta);
};