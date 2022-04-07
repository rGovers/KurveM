#pragma once

#include "ToolActions/ToolAction.h"

class PathModel;
class ShapeEditor;
class Workspace;

#include "AxisControl.h"

class MoveShapeNodeToolAction : public ToolAction
{
private:
    Workspace*   m_workspace;
    ShapeEditor* m_shapeEditor;

    bool Interact(const glm::mat4& a_viewProj, const glm::vec2& a_pos, e_Axis a_axis, const Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, PathModel* a_model);

protected:

public:
    MoveShapeNodeToolAction(Workspace* a_workspace, ShapeEditor* a_shapeEditor);
    virtual ~MoveShapeNodeToolAction();

    virtual bool LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize);
    virtual bool LeftDown(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize);
    virtual bool LeftReleased(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize);

    virtual void Draw(Camera* a_camera);
};