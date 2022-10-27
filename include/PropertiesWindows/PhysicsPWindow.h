#pragma once

#include "PropertiesWindows/PWindow.h"

#include "Physics/CollisionObjects/CollisionObject.h"
#include "Windows/PropertiesWindow.h"

class Editor;
class Softbody;
class Workspace;

class PhysicsPWindow : public PWindow
{
private:
    static constexpr char* CollisionObjectString[] = { "Null", "Collision Object", "Rigidbody", "Softbody" };
    static constexpr char* CollisionShapeString[] = { "Null", "Box", "Capsule", "Mesh", "Plane", "Sphere" };

    Workspace*     m_workspace;
    Editor*        m_editor;

    void LineStiffness(const char* a_displayName, Object* const* a_objs, unsigned int a_objectCount, const Softbody* a_body) const;
    void LineAngularStiffness(const char* a_displayName, Object* const* a_objs, unsigned int a_objectCount, const Softbody* a_body) const;
    void LineVolumeStiffness(const char* a_displayName, Object* const* a_objs, unsigned int a_objectCount, const Softbody* a_body) const;

    bool DisplayCollisionObjectOption(e_CollisionObjectType a_type, const Object* a_object) const;

protected:

public:
    PhysicsPWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~PhysicsPWindow();

    virtual e_ObjectPropertiesTab GetWindowType() const;
    virtual void Update();
};
