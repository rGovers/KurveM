#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Editor.h"

class Camera;
class Object;

class ControlEditor
{
private:
    
protected:

public:
    virtual ~ControlEditor() {}
    
    virtual e_EditorMode GetEditorMode() = 0;
    
    virtual void DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize) = 0;

    virtual void LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize) = 0;
    virtual void LeftDown(double a_delta, Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_currentPos, const glm::vec2& a_winSize) = 0;
    virtual void LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize) = 0;

    virtual void Update(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, double a_delta) = 0;
};