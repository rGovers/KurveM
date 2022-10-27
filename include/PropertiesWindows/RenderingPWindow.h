#pragma once

#include "PropertiesWindows/PWindow.h"

#include <list>

#include "Windows/PropertiesWindow.h"

class ShaderSettings;
class Workspace;

class RenderingPWindow : public PWindow
{
private:
    static constexpr char* VertexInputTypeString[] = { "Null", "Float", "Vec2", "Vec3", "Vec4" };
    static constexpr char* VertexInputSlotString[] = { "Null", "Position", "Normal", "UV", "Bone", "Weight", "Softbody Index", "Softbody Weight" };

    static constexpr char* ShaderInputTypeInputString[] = { "Null", "Value", "Camera View", "Camera Projection", "Transform" };
    static constexpr char* ShaderInputTypeString[] = { "Null", "Mat3", "Mat4" };

    Workspace* m_workspace;

protected:

public:
    RenderingPWindow(Workspace* a_workspace);
    virtual ~RenderingPWindow();

    virtual e_ObjectPropertiesTab GetWindowType() const;
    virtual void Update();
};
