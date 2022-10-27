#pragma once

#include "Windows/Window.h"

class Editor;
class PWindow;
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
    ObjectPropertiesTab_Null = -1,
    ObjectPropertiesTab_Animate,
    ObjectPropertiesTab_Curve,
    ObjectPropertiesTab_Object,
    ObjectPropertiesTab_Path,
    ObjectPropertiesTab_Physics,
    ObjectPropertiesTab_Rendering,
    ObjectPropertiesTab_End
};

class PropertiesWindow : public Window
{
private:
    static constexpr char* AnimateTooltip = "Contains object animation settings";
    static constexpr char* ObjectTooltip = "Contains object settings";
    static constexpr char* CurveTooltip = "Contains curve model settings";
    static constexpr char* PathTooltip = "Contains path model settings";
    static constexpr char* PhysicsTooltip = "Contains object physics settings";
    static constexpr char* RenderingTooltip = "Contains object rendering settings";

    PWindow**             m_windows;

    Workspace*            m_workspace;
    Editor*               m_editor;

    e_ObjectPropertiesTab m_propertiesMode;

    void RotationModeDisplay();

    void PropertiesTabButton(const char* a_label, const char* a_path, e_ObjectPropertiesTab a_propertiesTab, const char* a_tooltip);

protected:

public:
    PropertiesWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~PropertiesWindow();

    virtual void Update(double a_delta);
};