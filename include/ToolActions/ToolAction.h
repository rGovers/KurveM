#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class Camera;

class ToolAction
{
private:
    
protected:

public:
    virtual ~ToolAction() {}

    virtual bool LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize) { return false; }
    virtual bool LeftDown(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize) { return false; }
    virtual bool LeftReleased(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize) { return false; }
    virtual void Draw(Camera* a_camera) {}
};