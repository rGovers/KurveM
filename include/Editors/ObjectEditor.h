#pragma once

#include "Editors/ControlEditor.h"

class Workspace;

class ObjectEditor : public ControlEditor
{
private:
    Editor*    m_editor;
    Workspace* m_workspace;

    bool IsInteractingTransform(Camera* a_camera, const glm::vec3& a_pos, const glm::vec3& a_axis, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, const glm::mat4& a_viewProj);

protected:

public:
    ObjectEditor(Editor* a_editor, Workspace* a_workspace);
    virtual ~ObjectEditor();

    virtual e_EditorMode GetEditorMode();

    virtual void DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize);

    virtual void LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize);
    virtual void LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize);

    virtual void Update(Camera* a_camera, const glm::vec2& a_screenSize, double a_delta);
};