#pragma once

#include <functional>

#include "Application.h"

class Editor;
class Object;
class Workspace;

class EditorInputController
{
private:
    Workspace*                     m_workspace;
    Editor*                        m_editor;
                
    bool*                          m_states;

    std::function<void(Object*)>** m_curveFunctions;
    std::function<void(Object*)>** m_pathFunctions;

    void KeyDown(GLFWwindow* a_window, int a_key, Object* a_obj);

protected:

public:
    EditorInputController(Workspace* a_workspace, Editor* a_editor);
    ~EditorInputController();

    void Update(double a_delta);
};