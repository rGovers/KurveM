#pragma once

#include "PropertiesWindows/PWindow.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>

#include "Windows/PropertiesWindow.h"

class Transform;
class Workspace;

class ObjectPWindow : public PWindow
{
private:
    static constexpr char* RotationModeString[RotationMode_End] = { "Axis Angle", "Quaternion", "Euler Angle" };

    Workspace*     m_workspace;
    
    Transform*     m_transform;
    
    glm::vec3      m_transformEuler;
    glm::vec4      m_transformAxisAngle;
    glm::quat      m_transformQuaternion;

    e_RotationMode m_rotationMode;

    void RotationModeDisplay();
protected:

public:
    ObjectPWindow(Workspace* a_workspace);
    virtual ~ObjectPWindow();

    virtual e_ObjectPropertiesTab GetWindowType() const;
    virtual void Update();
};