#include "EditorInputController.h"

#include "EditorControls/Editor.h"
#include "Object.h"
#include "Workspace.h"

EditorInputController::EditorInputController(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_states = new bool[GLFW_KEY_LAST];

    m_curveFunctions = new std::function<void(Object*)>*[GLFW_KEY_LAST];
    m_pathFunctions = new std::function<void(Object*)>*[GLFW_KEY_LAST];

    for (int i = 0; i < GLFW_KEY_LAST; ++i)
    {
        m_curveFunctions[i] = nullptr;
        m_pathFunctions[i] = nullptr;
    }

    m_curveFunctions[GLFW_KEY_DELETE] = new std::function<void(Object*)>(std::bind(&Editor::CurveDeleteDown, m_editor, std::placeholders::_1));
    m_curveFunctions[GLFW_KEY_F] = new std::function<void(Object*)>(std::bind(&Editor::CurveFDown, m_editor, std::placeholders::_1));
    m_curveFunctions[GLFW_KEY_N] = new std::function<void(Object*)>(std::bind(&Editor::CurveNDown, m_editor, std::placeholders::_1));

    m_pathFunctions[GLFW_KEY_DELETE] = new std::function<void(Object*)>(std::bind(&Editor::PathDeleteDown, m_editor, std::placeholders::_1));
}
EditorInputController::~EditorInputController()
{
    delete[] m_states;

    delete[] m_curveFunctions;
    delete[] m_pathFunctions;
}

void EditorInputController::KeyDown(GLFWwindow* a_window, int a_key, Object* a_obj)
{
    if (glfwGetKey(a_window, a_key))
    {
        if (!m_states[a_key])
        {
            switch (a_obj->GetObjectType())
            {
            case ObjectType_CurveModel:
            {
                if (m_curveFunctions[a_key] != nullptr)
                {
                    (*m_curveFunctions[a_key])(a_obj);
                }

                break;
            }
            case ObjectType_PathModel:
            {
                if (m_pathFunctions[a_key] != nullptr)
                {
                    (*m_pathFunctions[a_key])(a_obj);
                }

                break;
            }
            }
        }

        m_states[a_key] = true;
    }
    else
    {
        m_states[a_key] = false;
    }
}

void EditorInputController::Update(double a_delta)
{
    const Application* app = Application::GetInstance();
    GLFWwindow* window = app->GetWindow();

    Object* obj = m_workspace->GetSelectedObject();

    if (obj != nullptr)
    {   
        // Start at 32 to avoid invalid keys
        // System is hacky but works
        for (int i = 32; i < GLFW_KEY_LAST; ++i)
        {
            KeyDown(window, i, obj);
        }
    }
}