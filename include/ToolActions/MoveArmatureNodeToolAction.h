#pragma once

#include "ToolActions/ToolAction.h"

class Editor;
class Workspace;

#include "AxisControl.h"

class MoveArmatureNodeToolAction : public ToolAction
{
private:
    Workspace* m_workspace;
    Editor*    m_editor;

protected:

public:
    MoveArmatureNodeToolAction(Workspace* a_workspace, Editor* a_editor);
    virtual ~MoveArmatureNodeToolAction();

    bool Interact(const glm::mat4& a_viewProj, const glm::vec3& a_pos, e_Axis a_axis, const Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize);

    virtual bool LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize);
    virtual bool LeftDown(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize);
    virtual bool LeftReleased(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize);

    virtual void Draw(Camera* a_camera);
};