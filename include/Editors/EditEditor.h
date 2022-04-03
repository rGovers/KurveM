#pragma once

#include "Editors/ControlEditor.h"

#include "AxisControl.h"

struct Node3Cluster;
struct PathNode;

class Camera;
class CurveModel;
class PathModel;
class ToolAction;
class Workspace;

class EditEditor : public ControlEditor
{
private:
    Workspace*   m_workspace;  
    Editor*      m_editor;

    ToolAction** m_armatureAction;
    ToolAction** m_curveAction;
    ToolAction** m_pathAction;

    bool SelectArmatureNodes(Object* a_node, const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end);

    bool IsInteractingCurveNodeHandle(const Node3Cluster& a_node, unsigned int a_nodeIndex, CurveModel* a_model, const glm::mat4& a_viewProj, const glm::vec2& a_cursorPos, const glm::mat4& a_transform, const glm::vec3& a_up, const glm::vec3& a_right);
    bool IsInteractingPathNodeHandle(const PathNode& a_node, unsigned int a_nodeIndex, PathModel* a_model, const glm::mat4& a_viewProj, const glm::vec2& a_cursorPos, const glm::mat4& a_transform, const glm::vec3& a_up, const glm::vec3& a_right);

protected:

public:
    EditEditor(Editor* a_editor, Workspace* a_workspace);
    virtual ~EditEditor();

    virtual e_EditorMode GetEditorMode();

    virtual void DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize);

    virtual void LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize);
    virtual void LeftDown(double a_delta, Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_currentPos, const glm::vec2& a_winSize);
    virtual void LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize);

    virtual void Update(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, double a_delta);
};