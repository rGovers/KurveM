#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>

#include "Animation.h"
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
    ObjectPropertiesTab_Animate,
    ObjectPropertiesTab_Curve,
    ObjectPropertiesTab_Object,
    ObjectPropertiesTab_End,
};

class PropertiesWindow : public Window
{
private:
    Workspace*            m_workspace;
    Editor*               m_editor;

    Object*               m_lastObject;
    float                 m_lastTime;
    Transform*            m_transform;
    AnimationNode         m_node;

    glm::vec3             m_transformEuler;
    glm::vec4             m_transformAxisAngle;
    glm::quat             m_transformQuaternion;

    glm::vec3             m_nodeEuler;
    glm::vec4             m_nodeAxisAngle;
    glm::quat             m_nodeQuaternion;

    e_RotationMode        m_rotationMode;

    e_ObjectPropertiesTab m_propertiesMode;

    void PushRotation(const glm::quat& a_quat);
    void PushAnimationNode(Animation* a_animation, Object* a_obj, const AnimationNode& a_node);

    void RotationModeDisplay();

    void ObjectTab();
    void AnimateTab();
    void CurveTab();

    void PropertiesTabButton(const char* a_label, const char* a_path, e_ObjectPropertiesTab a_propertiesTab, const char* a_tooltip);

protected:

public:
    PropertiesWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~PropertiesWindow();

    virtual void Update(double a_delta);
};