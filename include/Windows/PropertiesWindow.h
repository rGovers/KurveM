#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>

#include "Windows/Window.h"

class Editor;
class Transform;
class Workspace;

enum e_RotationMode
{
    RotationMode_AxisAngle,
    RotationMode_Quaternion,
    RotationMode_EulerAngle,
    RotationMode_End
};
enum e_ObjectPropertiesTab
{
    ObjectPropertiesTab_Object,
    ObjectPropertiesTab_Curve,
    ObjectPropertiesTab_End,
};

class PropertiesWindow : public Window
{
private:
    Workspace*            m_workspace;
    Editor*               m_editor;

    Transform*            m_transform;

    glm::vec3             m_euler;
    glm::vec4             m_axisAngle;
    glm::quat             m_quaternion;

    e_RotationMode        m_rotationMode;

    e_ObjectPropertiesTab m_propertiesMode;

    void PushRotation(const glm::quat& a_quat);

    void PropertiesTabButton(const char* a_label, const char* a_path, e_ObjectPropertiesTab a_propertiesTab, const char* a_tooltip);

protected:

public:
    PropertiesWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~PropertiesWindow();

    virtual void Update(double a_delta);
};