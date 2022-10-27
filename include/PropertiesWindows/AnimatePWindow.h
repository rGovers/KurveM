#pragma once

#include "PropertiesWindows/PWindow.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>

#include "Animation.h"
#include "Windows/PropertiesWindow.h"

class Editor;
class Object;
class Transform;
class Workspace;

class AnimatePWindow : public PWindow
{
private:
    static constexpr char* RotationModeString[RotationMode_End] = { "Axis Angle", "Quaternion", "Euler Angle" };

    Workspace*     m_workspace;
    Editor*        m_editor;

    Object*        m_lastObject;
    float          m_lastTime;
    AnimationNode  m_node;

    Transform*     m_transform;
    
    glm::vec3      m_transformEuler;
    glm::vec4      m_transformAxisAngle;
    glm::quat      m_transformQuaternion;

    e_RotationMode m_rotationMode;

    void RotationModeDisplay();
protected:

public:
    AnimatePWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~AnimatePWindow();

    virtual e_ObjectPropertiesTab GetWindowType() const;
    virtual void Update();
};