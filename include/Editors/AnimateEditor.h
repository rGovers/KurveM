#pragma once

#include "Editors/ControlEditor.h"

class Workspace;

class AnimateEditor : public ControlEditor
{
private:
    Editor*    m_editor;

    Workspace* m_workspace;

protected:

public:
    AnimateEditor(Editor* a_editor, Workspace* a_workspace);
    virtual ~AnimateEditor();
    
    virtual e_EditorMode GetEditorMode();
    
    virtual void DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize);

    virtual void LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize);
    virtual void LeftDown(double a_delta, Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_currentPos, const glm::vec2& a_winSize);
    virtual void LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize);

    virtual void Update(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, double a_delta);
};