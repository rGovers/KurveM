#include "Editors/WeightPaintingEditor.h"

#include "Actions/AddCurveNodeWeightAction.h"
#include "Camera.h"
#include "CurveModel.h"
#include "Gizmos.h"
#include "Object.h"
#include "SelectionControl.h"
#include "Transform.h"
#include "Workspace.h"

WeightPaintingEditor::WeightPaintingEditor(Editor* a_editor, Workspace* a_workspace)
{
    m_editor = a_editor;

    m_workspace = a_workspace;
}
WeightPaintingEditor::~WeightPaintingEditor()
{

}

e_EditorMode WeightPaintingEditor::GetEditorMode()
{
    return EditorMode_WeightPainting;
}

void WeightPaintingEditor::DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize)
{
    if (a_object == m_workspace->GetSelectedObject())
    {
        const e_ObjectType objectType = a_object->GetObjectType();

        unsigned int index = 0;
        unsigned int size = 0;

        switch (objectType)
        {
        case ObjectType_CurveModel:
        {
            const CurveModel* model = a_object->GetCurveModel();

            const std::list<Object*> nodes = model->GetArmatureNodes();
            size = nodes.size();

            const long long selectedID = m_editor->GetSelectedWeightNode();

            unsigned int iterIndex = 0;
            for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
            {
                const Object* obj = *iter;
                if (obj->GetID() == selectedID)
                {
                    index = iterIndex;

                    break;
                }

                ++iterIndex;
            }
        }
        }

        a_object->DrawWeight(a_camera, a_winSize, index, size);
    }
}

void WeightPaintingEditor::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize)
{
    const Object* obj = m_workspace->GetSelectedObject();

    if (obj != nullptr)
    {
        const e_ObjectType objectType = obj->GetObjectType();

        switch (objectType)
        {
        case ObjectType_CurveModel:
        {
            CurveModel* model = obj->GetCurveModel();

            if (model != nullptr)
            {
                Action* action = new AddCurveNodeWeightAction(model, m_workspace);
                if (!m_workspace->PushAction(action))
                {
                    printf("Cannot paint weights \n");

                    delete action;
                }
                else
                {
                    m_editor->SetCurrentAction(action);
                }
            }

            break;
        }
        }
    }

}
void WeightPaintingEditor::LeftDown(double a_delta, Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_currentPos, const glm::vec2& a_winSize)
{
    const glm::mat4 view = a_camera->GetView();
    const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);
    const glm::mat4 viewProj = proj * view;

    const Object* obj = m_workspace->GetSelectedObject();

    if (obj != nullptr)
    {
        const e_ObjectType objectType = obj->GetObjectType();

        switch (objectType)
        {
        case ObjectType_CurveModel:
        {
            CurveModel* model = obj->GetCurveModel();

            if (model != nullptr)
            {
                Action* action = m_editor->GetCurrentAction();

                const e_ActionType actionType = action->GetActionType();

                switch (actionType)
                {
                case ActionType_AddCurveNodeWeight:
                {
                    long long weightNode = m_editor->GetSelectedWeightNode();

                    if (weightNode == -1)
                    {
                        Object* arm = model->GetArmature();
                        if (arm != nullptr)
                        {
                            const std::list<Object*> armNodes = arm->GetChildren();

                            for (auto iter = armNodes.begin(); iter != armNodes.end(); ++iter)
                            {
                                const Object* armObj = *iter;
                                if (armObj != nullptr && armObj->GetObjectType() == ObjectType_ArmatureNode)
                                {
                                    weightNode = armObj->GetID();

                                    m_editor->SetSelectedWeightNode(weightNode);

                                    break;
                                }
                            }
                        }   
                        else 
                        {
                            printf("Invalid armature \n");
                        }
                    }

                    AddCurveNodeWeightAction* weightAction = (AddCurveNodeWeightAction*)action;

                    Node3Cluster* nodes = model->GetNodes();
                    const unsigned int nodeCount = model->GetNodeCount();

                    const float intensity = m_editor->GetBrushIntensity();
                    const float radius = m_editor->GetBrushRadius() / 10.0f;

                    bool change = false;
                    for (unsigned int i = 0; i < nodeCount; ++i)
                    {
                        if (SelectionControl::PointInPoint(viewProj, a_currentPos, radius, nodes[i].Nodes.begin()->Node.GetPosition()))
                        {
                            change = true;
                            weightAction->AddNodeDelta(i, weightNode, (float)(intensity * a_delta));
                        }
                    }

                    if (change)
                    {
                        weightAction->Execute();
                    }

                    break;
                }
                }
            }

            break;
        }
        }
    }
}
void WeightPaintingEditor::LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize)
{

}

void WeightPaintingEditor::Update(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, double a_delta)
{    
    const Transform* camTransform = a_camera->GetTransform();
    const glm::mat4 viewInv = camTransform->ToMatrix();

    const glm::vec3 cPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, -0.99f), (int)a_screenSize.x, (int)a_screenSize.y);

    // const float radius = m_editor->GetBrushRadius() / 20.0f;

    // Gizmos::DrawCircle(cPos, viewInv[2].xyz(), radius, 0.0001f, 30, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
}