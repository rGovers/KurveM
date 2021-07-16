#include "EditorInputController.h"

#include "Object.h"
#include "Workspace.h"

EditorInputController::EditorInputController(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
EditorInputController::~EditorInputController()
{

}

void EditorInputController::KeyDown(GLFWwindow* a_window, int a_key, bool* a_state, Object* a_obj, const std::function<void(Object*)>& a_curveFunction) const
{
    if (glfwGetKey(a_window, a_key))
    {
        if (!*a_state)
        {
            switch (a_obj->GetObjectType())
            {
            case ObjectType_CurveModel:
            {
                a_curveFunction(a_obj);

                break;
            }
            }
        }

        *a_state = true;
    }
    else
    {
        *a_state = false;
    }
}

void EditorInputController::Update(double a_delta)
{
    const Application* app = Application::GetInstance();
    GLFWwindow* window = app->GetWindow();

    Object* obj = m_workspace->GetSelectedObject();

    if (obj != nullptr)
    {   
        KeyDown(window, GLFW_KEY_DELETE, &m_deleteDown, obj, std::bind(&Editor::CurveDeleteDown, m_editor, std::placeholders::_1));
        KeyDown(window, GLFW_KEY_F, &m_fDown, obj, std::bind(&Editor::CurveFDown, m_editor, std::placeholders::_1));
        KeyDown(window, GLFW_KEY_N, &m_nDown, obj, std::bind(&Editor::CurveNDown, m_editor, std::placeholders::_1));
    }
}