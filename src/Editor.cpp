#include "EditorControls/Editor.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>
#include <stdio.h>

#include "Actions/ExtrudeArmatureNodeAction.h"
#include "Actions/FlipFaceAction.h"
#include "Actions/InsertFaceAction.h"
#include "Actions/MoveCurveNodeHandleAction.h"
#include "Actions/MovePathNodeHandleAction.h"
#include "Actions/RotateObjectRelativeAction.h"
#include "Actions/ScaleObjectRelativeAction.h"
#include "Actions/TranslateObjectRelativeAction.h"
#include "Application.h"
#include "BezierCurveNode3.h"
#include "Camera.h"
#include "CameraController.h"
#include "ColorTheme.h"
#include "CurveModel.h"
#include "Datastore.h"
#include "EditorInputController.h"
#include "Editors/AnimateEditor.h"
#include "Editors/EditEditor.h"
#include "Editors/ObjectEditor.h"
#include "Editors/WeightPaintingEditor.h"
#include "Gizmos.h"
#include "imgui.h"
#include "Modals/DeleteNodesModal.h"
#include "Model.h"
#include "Object.h"
#include "RenderTexture.h"
#include "ShaderPixel.h"
#include "ShaderProgram.h"
#include "Shaders/GridPixel.h"
#include "Shaders/GridVertex.h"
#include "ShaderVertex.h"
#include "SelectionControl.h"
#include "Transform.h"
#include "Workspace.h"

Editor::Editor(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_renderTexture = new RenderTexture(640, 480);

    m_mouseDown = 0;

    m_camera = new Camera();
    m_camera->SetFOV(0.6911504f);

    m_brushRadius = 1;
    m_brushIntensity = 1;

    Init();

    m_cameraController = new CameraController(m_camera);

    m_inputController = new EditorInputController(m_workspace, this);

    m_editorControls.emplace_back(new AnimateEditor(this, m_workspace));
    m_editorControls.emplace_back(new EditEditor(this, m_workspace));
    m_editorControls.emplace_back(new ObjectEditor(this, m_workspace));
    m_editorControls.emplace_back(new WeightPaintingEditor(this, m_workspace));
}
Editor::~Editor()
{
    for (auto iter = m_editorControls.begin(); iter != m_editorControls.end(); ++iter)
    {
        delete *iter;
    }

    delete m_renderTexture;

    delete m_cameraController;
    delete m_camera;
}

void Editor::Init()
{
    m_gridShader = Datastore::GetShaderProgram("SHADER_GRID");
    if (m_gridShader == nullptr)
    {   
        ShaderVertex* vertexShader = new ShaderVertex(GRIDVERTEX);
        ShaderPixel* pixelShader = new ShaderPixel(GRIDPIXEL);

        m_gridShader = new ShaderProgram(vertexShader, pixelShader);

        delete vertexShader;
        delete pixelShader;

        Datastore::AddShaderProgram("SHADER_GRID", m_gridShader);
    }

    Transform* transform = m_camera->GetTransform();
    transform->Translation() = { 0.0f, -2.5f, -10.0f };
    transform->Quaternion() = glm::angleAxis(3.14159f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(3.14159f * 0.1f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));

    m_curAction = nullptr;

    m_selectedTime = 0.0f;
    m_animTime = 0.0f;

    m_editorMode = EditorMode_Object;
    m_faceCullingMode = EditorFaceCullingMode_Back;
}

bool Editor::IsFaceSelected() const
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const CurveModel* curveModel = obj->GetCurveModel();
        if (curveModel != nullptr)
        {
            const unsigned int nodeCount = m_selectedNodes.size();

            unsigned int* nodes = new unsigned int[nodeCount];

            unsigned int index = 0;
            for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
            {
                nodes[index++] = *iter;
            }

            bool ret = false;

            switch (nodeCount)
            {
            case 3:
            {
                ret = curveModel->Get3PointFaceIndex(nodes[0], nodes[1], nodes[2]) != -1;

                break;
            }
            case 6:
            {
                ret = curveModel->Get3PointFaceIndex(nodes) != -1;

                break;
            }
            case 4:
            {
                ret = curveModel->Get4PointFaceIndex(nodes[0], nodes[1], nodes[2], nodes[3]) != -1;

                break;
            }
            case 8:
            {
                ret = curveModel->Get4PointFaceIndex(nodes) != -1;

                break;
            }
            }

            delete[] nodes;

            return ret;
        }
    }

    return false;
}
bool Editor::CanInsertFace() const
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const CurveModel* curveModel = obj->GetCurveModel();
        if (curveModel != nullptr)
        {
            const unsigned int nodeCount = m_selectedNodes.size();

            return nodeCount == 3 || nodeCount == 4;
        }
    }

    return false;
}

void Editor::SetSelectedWeightNode(Object* a_value)
{
    m_selectedWeightNode = -1;

    if (a_value != nullptr && a_value->GetObjectType() == ObjectType_ArmatureNode)
    {
        m_selectedWeightNode = a_value->GetID();
    }
}

long long* Editor::GetSelectedArmatureNodesArray() const
{
    const int size = m_selectedArmNodes.size();
    long long* ids = new long long[size];

    int index = 0;
    for (auto iter = m_selectedArmNodes.begin(); iter != m_selectedArmNodes.end(); ++iter)
    {
        ids[index++] = *iter;
    }

    return ids;
}
Object** Editor::GetSelectedArmatureObjectsArray() const
{
    const int arrSize = m_selectedArmNodes.size();

    Object** objs = new Object*[arrSize];

    int index = 0;
    for (auto iter = m_selectedArmNodes.begin(); iter != m_selectedArmNodes.end(); ++iter)
    {
        Object* obj = m_workspace->GetObject(*iter);
        if (obj != nullptr)
        {
            objs[index++] = obj;
        }
    }

    return objs;
}
int Editor::GetSelectedArmatureObjectsCount() const
{
    int count = 0;
    for (auto iter = m_selectedArmNodes.begin(); iter != m_selectedArmNodes.end(); ++iter)
    {
        Object* obj = m_workspace->GetObject(*iter);
        if (obj != nullptr)
        {
            ++count;
        }
    }

    return count;
}

unsigned int* Editor::GetSelectedNodesArray() const
{
    const unsigned int nodeCount = m_selectedNodes.size();

    unsigned int* nodes = new unsigned int[nodeCount];

    unsigned int index = 0;
    for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
    {
        nodes[index++] = *iter;
    }

    return nodes;
}

void Editor::SetEditorMode(e_EditorMode a_editorMode)
{
    m_editorMode = a_editorMode;

    m_selectedWeightNode = -1;
}

bool Editor::IsEditorModeEnabled(e_EditorMode a_editorMode) const
{
    switch (a_editorMode)
    {
        case EditorMode_Edit:
        {
            if (m_workspace->GetSelectedObjectCount() == 1)
            {
                const Object* obj = m_workspace->GetSelectedObject();

                switch (obj->GetObjectType())
                {
                case ObjectType_Armature:
                case ObjectType_CurveModel:
                case ObjectType_PathModel:
                {
                    return obj->IsVisible();
                }
                }                
            }

            return false;
        }
        case EditorMode_WeightPainting:
        {
            if (m_workspace->GetSelectedObjectCount() == 1)
            {
                const Object* obj = m_workspace->GetSelectedObject();

                switch (obj->GetObjectType())
                {
                case ObjectType_CurveModel:
                {
                    const CurveModel* model = obj->GetCurveModel();
                    if (model != nullptr)
                    {
                        return model->GetArmatureID() != -1;
                    }
                }
                }
            }
        }
        case EditorMode_End:
        {
            return false;
        }
    }

    return true;
}

void Editor::ClearSelectedNodes()
{
    m_selectedNodes.clear();
    m_selectedArmNodes.clear();
}
void Editor::AddNodeToSelection(unsigned int a_index)
{
    for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
    {
        if (*iter == a_index)
        {
            return;
        }
    }

    m_selectedNodes.emplace_back(a_index);
}
void Editor::RemoveNodeFromSelection(unsigned int a_index)
{
    for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
    {
        if (*iter == a_index)
        {
            m_selectedNodes.erase(iter);

            return;
        }
    }
}

void Editor::AddArmatureNodeToSelection(long long a_id)
{
    for (auto iter = m_selectedArmNodes.begin(); iter != m_selectedArmNodes.end(); ++iter)
    {
        if (*iter == a_id)
        {
            return;
        }
    }

    m_selectedArmNodes.emplace_back(a_id);
}
void Editor::AddArmatureNodeToSelection(Object* a_object)
{
    if (a_object != nullptr)
    {
        AddArmatureNodeToSelection(a_object->GetID());
    }
}
void Editor::RemoveArmatureNodeFromSelection(long long a_id)
{
    for (auto iter = m_selectedArmNodes.begin(); iter != m_selectedArmNodes.end(); ++iter)
    {
        if (*iter == a_id)
        {
            m_selectedArmNodes.erase(iter);

            return;
        }
    }
}
void Editor::RemoveArmatureNodeFromSelection(Object* a_object)
{
    if (a_object != nullptr)
    {
        RemoveArmatureNodeFromSelection(a_object->GetID());
    }
}

void Editor::DrawObject(Object* a_object, const glm::vec2& a_winSize)
{
    for (auto iter = m_editorControls.begin(); iter != m_editorControls.end(); ++iter)
    {
        ControlEditor* editor = *iter;
        if (editor->GetEditorMode() == m_editorMode)
        {
            editor->DrawObject(m_camera, a_object, a_winSize);
        }
    }

    const std::list<Object*> children = a_object->GetChildren();

    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        DrawObject(*iter, a_winSize);
    }
}

e_ActionType Editor::GetCurrentActionType() const
{
    if (m_curAction != nullptr)
    {
        return m_curAction->GetActionType();
    }

    return ActionType_Null;
}

void Editor::CurveDeleteDown(Object* a_object)
{
    CurveModel* model = a_object->GetCurveModel();
    if (model != nullptr)
    {
        const unsigned int size = m_selectedNodes.size();
        if (size > 0)
        {
            unsigned int* indices = GetSelectedNodesArray();

            m_workspace->PushModal(new DeleteNodesModal(m_workspace, this, indices, size, model));

            delete[] indices;
        }
    }
}
void Editor::CurveFDown(Object* a_object)
{
    CurveModel* model = a_object->GetCurveModel();

    if (model != nullptr)
    {
        const unsigned int indexCount = m_selectedNodes.size();
        unsigned int* indices = GetSelectedNodesArray();

        m_curAction = new InsertFaceAction(m_workspace, indices, indexCount, model);
        if (!m_workspace->PushAction(m_curAction))
        {
            printf("Cannot insert face \n");

            delete m_curAction;
            m_curAction = nullptr;
        }

        delete[] indices;
    }
}
void Editor::CurveNDown(Object* a_object)
{
    CurveModel* model = a_object->GetCurveModel();
    if (model != nullptr)
    {
        const unsigned int size = m_selectedNodes.size();

        unsigned int* indices = GetSelectedNodesArray();

        m_curAction = new FlipFaceAction(m_workspace, indices, size, model);
        if (!m_workspace->PushAction(m_curAction))
        {
            printf("Cannot flip face \n");

            delete m_curAction;
            m_curAction = nullptr;
        }

        delete[] indices;
    }
}

void Editor::Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize)
{
    if (a_winSize.x != m_renderTexture->GetWidth() || a_winSize.y != m_renderTexture->GetHeight())
    {
        m_renderTexture->Resize(a_winSize.x, a_winSize.y);
    }

    const Application* app = Application::GetInstance();
    GLFWwindow* window = app->GetWindow();
    const ImGuiIO& io = ImGui::GetIO();

    if (!IsEditorModeEnabled(m_editorMode))
    {
        SetEditorMode(EditorMode_Object);
    }

    const glm::vec2 halfWinSize = a_winSize * 0.5f;

    Gizmos::Clear();

    const ImVec2 mousePos = io.MousePos; 
    const glm::vec2 cursorPos = glm::vec2(mousePos.x, mousePos.y);

    Transform* camTransform = m_camera->GetTransform();

    const glm::mat4 viewInv = camTransform->ToMatrix();
    const glm::mat4 view = glm::inverse(viewInv);

    const glm::mat4 proj = m_camera->GetProjection(a_winSize.x, a_winSize.y);

    const glm::vec2 curCursorPos = (cursorPos - (a_winPos + halfWinSize)) / halfWinSize;

    Object* object = m_workspace->GetSelectedObject();

    if (ImGui::IsWindowFocused())
    {
        m_inputController->Update(a_delta);
         
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            if (!(m_mouseDown & 0b1 << 0))
            {
                m_startPos = curCursorPos;

                switch (GetCurrentActionType())
                {
                case ActionType_ExtrudeArmatureNode:
                case ActionType_RotateObjectRelative:
                case ActionType_TranslateObjectRelative:
                {
                    m_curAction = nullptr;

                    break;
                }
                }

                for (auto iter = m_editorControls.begin(); iter != m_editorControls.end(); ++iter)
                {
                    ControlEditor* editor = *iter;
                    if (editor->GetEditorMode() == m_editorMode)
                    {
                        editor->LeftClicked(m_camera, curCursorPos, a_winSize);

                        break;
                    }
                }
            }

            m_mouseDown |= 0b1 << 0;
        }
        
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            m_mouseDown |= 0b1 << 1;
        }   

        m_cameraController->FocusUpdate();       
    }

    if (m_mouseDown & 0b1 << 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_mouseDown &= ~(0b1 << 0);

        for (auto iter = m_editorControls.begin(); iter != m_editorControls.end(); ++iter)
        {
            ControlEditor* editor = *iter;
            if (editor->GetEditorMode() == m_editorMode)
            {
                editor->LeftReleased(m_camera, m_startPos, curCursorPos, a_winSize);
            }
        }
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
    {
        m_mouseDown &= ~(0b1 << 1);
    }

    if (m_mouseDown & 0b1 << 0)
    {
        const glm::vec3 cWorldPos = m_camera->GetScreenToWorld(glm::vec3(curCursorPos, 0.9f), (int)a_winSize.x, (int)a_winSize.y);

        const e_ActionType actionType = GetCurrentActionType();

        switch (actionType)
        {
        case ActionType_ExtrudeArmatureNode:
        {
            ExtrudeArmatureNodeAction* action = (ExtrudeArmatureNodeAction*)m_curAction;
            action->SetPosition(cWorldPos);

            action->Execute();

            break;
        }
        case ActionType_MoveCurveNodeHandle:
        {
            MoveCurveNodeHandleAction* action = (MoveCurveNodeHandleAction*)m_curAction;
            action->SetCursorPos(curCursorPos);

            action->Execute();

            break;
        }
        case ActionType_MovePathNodeHandle:
        {
            MovePathNodeHandleAction* action = (MovePathNodeHandleAction*)m_curAction;
            action->SetCursorPos(curCursorPos);

            action->Execute();

            break;
        }
        case ActionType_TranslateObjectRelative:
        {
            TranslateObjectRelativeAction* action = (TranslateObjectRelativeAction*)m_curAction;
            action->SetTranslation(cWorldPos);

            action->Execute();

            break;
        }
        case ActionType_RotateObjectRelative:
        {
            RotateObjectRelativeAction* action = (RotateObjectRelativeAction*)m_curAction;
            action->SetRotation(cWorldPos);

            action->Execute();

            break;
        }
        case ActionType_ScaleObjectRelative:
        {
            ScaleObjectRelativeAction* action = (ScaleObjectRelativeAction*)m_curAction;
            action->SetScale(cWorldPos);

            action->Execute();

            break;
        }
        }

        for (auto iter = m_editorControls.begin(); iter != m_editorControls.end(); ++iter)
        {
            ControlEditor* editor = *iter;
            if (editor->GetEditorMode() == m_editorMode)
            {
                editor->LeftDown(a_delta, m_camera, m_startPos, curCursorPos, a_winSize);

                break;
            }
        } 
    }

    if (m_mouseDown & 0b1 << 1)
    {
        m_cameraController->Update(a_delta);
    }

    for (auto iter = m_editorControls.begin(); iter != m_editorControls.end(); ++iter)
    {
        ControlEditor* editor = *iter;
        if (editor->GetEditorMode() == m_editorMode)
        {
            editor->Update(m_camera, curCursorPos, a_winSize, a_delta);

            break;
        }
    }

    const int width = m_renderTexture->GetWidth();
    const int height = m_renderTexture->GetHeight();

    const unsigned int handle = m_renderTexture->GetHandle();

    glm::vec4 viewCache;
    int fbCache;

    glGetFloatv(GL_VIEWPORT, (float*)&viewCache);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&fbCache);

    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClearColor(163.0f / 255, 163.0f / 255, 162.0f / 255, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  
    
    switch (m_faceCullingMode)
    {
        case EditorFaceCullingMode_Back:
        {
            glEnable(GL_CULL_FACE);  
            glCullFace(GL_BACK);  

            break;
        }
        case EditorFaceCullingMode_Front:
        {
            glEnable(GL_CULL_FACE);  
            glCullFace(GL_FRONT); 

            break;
        }
        case EditorFaceCullingMode_None:
        {
            glDisable(GL_CULL_FACE);

            break;
        }
        case EditorFaceCullingMode_All:
        {
            glEnable(GL_CULL_FACE);  
            glCullFace(GL_FRONT_AND_BACK); 

            break;
        }
    }

    const std::list<Object*> objs = m_workspace->GetObjectList();

    for (auto iter = objs.begin(); iter != objs.end(); ++iter)
    {
        Object* obj = *iter;

        DrawObject(obj, a_winSize);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);  

    const unsigned int programHandle = m_gridShader->GetHandle();
    glUseProgram(programHandle);

    glBindVertexArray(Model::GetEmpty()->GetVAO());

    glUniformMatrix4fv(0, 1, false, (float*)&view);
    glUniformMatrix4fv(1, 1, false, (float*)&proj);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisable(GL_BLEND);

    Gizmos::DrawAll(m_camera, a_winSize);

    // Restore framebuffer state for imgui
    glBindFramebuffer(GL_FRAMEBUFFER, fbCache);
    glViewport(viewCache[0], viewCache[1], viewCache[2], viewCache[3]);

    glEnable(GL_CULL_FACE);  
}