#include "ToolActions/MoveShapeNodeToolAction.h"

#include "Actions/MoveShapeNodeAction.h"
#include "Camera.h"
#include "EditorControls/ShapeEditor.h"
#include "Gizmos.h"
#include "Object.h"
#include "PathModel.h"
#include "SelectionControl.h"
#include "Workspace.h"

MoveShapeNodeToolAction::MoveShapeNodeToolAction(Workspace* a_workspace, ShapeEditor* a_shapeEditor)
{
    m_workspace = a_workspace;
    m_shapeEditor = a_shapeEditor;
}
MoveShapeNodeToolAction::~MoveShapeNodeToolAction()
{

}

bool MoveShapeNodeToolAction::Interact(const glm::mat4& a_viewProj, const glm::vec2& a_pos, e_Axis a_axis, const Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, PathModel* a_model)
{
    const glm::vec3 axis = AxisControl::GetAxis(a_axis) * 0.5f;
  
    glm::vec4 fPos = a_viewProj * glm::vec4(glm::vec3(a_pos.x, 0.0f, a_pos.y) + glm::vec3(axis.x, 0.0f, axis.y), 1.0f);
    fPos /= fPos.w;

    if (SelectionControl::PointInPoint(fPos.xy(), a_cursorPos, 0.05f))
    {
        const unsigned int nodeCount = m_shapeEditor->GetSelectedIndicesCount();
        const unsigned int* indices = m_shapeEditor->GetSelectedIndicesArray();

        Action* action = new MoveShapeNodeAction(m_workspace, indices, nodeCount, a_model, a_cursorPos, axis);
        if (m_workspace->PushAction(action))
        {
            m_shapeEditor->SetCurrentAction(action);
        }
        else
        {
            printf("Error moving shape node \n");

            delete action;
        }

        delete[] indices;

        return true;
    }

    return false;
}

bool MoveShapeNodeToolAction::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        PathModel* model = obj->GetPathModel();
        if (model != nullptr)
        {
            const ShapeNodeCluster* nodes = model->GetShapeNodes();

            const glm::mat4 view = a_camera->GetView();
            const glm::mat4 proj = a_camera->GetProjection(a_screenSize * m_shapeEditor->GetCameraZoom());
            const glm::mat4 viewProj = proj * view;

            const unsigned int indexCount = m_shapeEditor->GetSelectedIndicesCount();
            const std::list<unsigned int> selectedIndices = m_shapeEditor->GetSelectedIndices();

            glm::vec2 pos = glm::vec2(0.0f);
            for (auto iter = selectedIndices.begin(); iter != selectedIndices.end(); ++iter)
            {
                pos += nodes[*iter].Nodes[0].GetPosition();
            }
            pos /= indexCount;

            if (Interact(viewProj, pos, Axis_X, a_camera, a_cursorPos, a_screenSize, model) ||
                Interact(viewProj, pos, Axis_Y, a_camera, a_cursorPos, a_screenSize, model))
            {
                return true;
            }
        }
    }

    return false;
}
bool MoveShapeNodeToolAction::LeftDown(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    if (m_shapeEditor->GetCurrentActionType() == ActionType_MoveShapeNode)
    {
        MoveShapeNodeAction* action = (MoveShapeNodeAction*)m_shapeEditor->GetCurrentAction();
        action->SetPosition(a_cursorPos);

        action->Execute();

        return true;
    }

    return false;
}
bool MoveShapeNodeToolAction::LeftReleased(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    if (m_shapeEditor->GetCurrentActionType() == ActionType_MoveShapeNode)
    {
        m_shapeEditor->SetCurrentAction(nullptr);

        return true;
    }

    return false;
}

void MoveShapeNodeToolAction::Draw(Camera* a_camera)
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const PathModel* model = obj->GetPathModel();
        if (model != nullptr)
        {
            const unsigned int indexCount = m_shapeEditor->GetSelectedIndicesCount();
            if (indexCount > 0)
            {
                const std::list<unsigned int> selectedIndices = m_shapeEditor->GetSelectedIndices();

                const ShapeNodeCluster *nodes = model->GetShapeNodes();

                glm::vec2 pos = glm::vec2(0.0f);
                for (auto iter = selectedIndices.begin(); iter != selectedIndices.end(); ++iter)
                {
                    pos += nodes[*iter].Nodes[0].GetPosition();
                }
                pos /= indexCount;

                const glm::vec3 pos3 = glm::vec3(pos.x, 0.0f, pos.y);
                const glm::vec3 up = pos3 + glm::vec3(0.0f, 0.0f, 0.5f);
                const glm::vec3 right = pos3 + glm::vec3(0.5f, 0.0f, 0.0f);

                Gizmos::DrawLine(pos3, up, 0.01f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                Gizmos::DrawLine(pos3, right, 0.01f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

                Gizmos::DrawTriangle(up, glm::vec3(0.0f, 0.0f, 1.0f), 0.05f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                Gizmos::DrawTriangle(right, glm::vec3(1.0f, 0.0f, 0.0f), 0.05f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            }
        }
    }
}