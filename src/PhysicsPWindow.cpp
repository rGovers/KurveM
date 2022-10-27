#include "PropertiesWindows/PhysicsPWindow.h"

#include "Actions/SetBoxCollisionShapeHalfExtentsAction.h"
#include "Actions/SetCapsuleCollisionShapeHeightAction.h"
#include "Actions/SetCapsuleCollisionShapeRadiusAction.h"
#include "Actions/SetCollisionObjectActiveAction.h"
#include "Actions/SetCollisionObjectTypeAction.h"
#include "Actions/SetCollisionShapeTypeAction.h"
#include "Actions/SetPlaneCollisionShapeDirectionAction.h"
#include "Actions/SetPlaneCollisionShapeDistanceAction.h"
#include "Actions/SetRigidbodyMassAction.h"
#include "Actions/SetSoftbodyDampeningAction.h"
#include "Actions/SetSoftbodyFaceAngularStiffnessAction.h"
#include "Actions/SetSoftbodyFaceStiffnessAction.h"
#include "Actions/SetSoftbodyFaceVolumeStiffnessAction.h"
#include "Actions/SetSoftbodyLineAngularStiffnessAction.h"
#include "Actions/SetSoftbodyLineStiffnessAction.h"
#include "Actions/SetSoftbodyLineVolumeStiffnessAction.h"
#include "Actions/SetSoftbodyMassAction.h"
#include "Actions/SetSphereCollisionShapeRadiusAction.h"
#include "Physics/CollisionObjects/Rigidbody.h"
#include "Physics/CollisionObjects/Softbody.h"
#include "Physics/CollisionShapes/BoxCollisionShape.h"
#include "Physics/CollisionShapes/CapsuleCollisionShape.h"
#include "Physics/CollisionShapes/PlaneCollisionShape.h"
#include "Physics/CollisionShapes/SphereCollisionShape.h"
#include "Workspace.h"

PhysicsPWindow::PhysicsPWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_editor = a_editor;
    m_workspace = a_workspace;
}
PhysicsPWindow::~PhysicsPWindow()
{

}

void PhysicsPWindow::LineStiffness(const char* a_displayName, Object* const* a_objs, unsigned int a_objectCount, const Softbody* a_body) const
{
    float stiffness = a_body->GetLineStiffness();
    if (ImGui::SliderFloat(a_displayName, &stiffness, 0.0f, 1.0f))
    {
        m_workspace->PushActionSet(new SetSoftbodyLineStiffnessAction(a_objs, a_objectCount, stiffness), &stiffness, "Error setting softbody line stiffness");
    }
}
void PhysicsPWindow::LineAngularStiffness(const char* a_displayName, Object* const* a_objs, unsigned int a_objectCount, const Softbody* a_body) const
{
    float stiffness = a_body->GetLineAngularStiffness();
    if (ImGui::SliderFloat(a_displayName, &stiffness, 0.0f, 1.0f))
    {
        m_workspace->PushActionSet(new SetSoftbodyLineAngularStiffnessAction(a_objs, a_objectCount, stiffness), &stiffness, "Error setting softbody line angular stiffness");
    }
}
void PhysicsPWindow::LineVolumeStiffness(const char* a_displayName, Object* const* a_objs, unsigned int a_objectCount, const Softbody* a_body) const
{
    float stiffness = a_body->GetLineVolumeStiffness();
    if (ImGui::SliderFloat(a_displayName, &stiffness, 0.0f, 1.0f))
    {
        m_workspace->PushActionSet(new SetSoftbodyLineVolumeStiffnessAction(a_objs, a_objectCount, stiffness), &stiffness, "Error setting softbody line volume stiffness");
    }
}

bool PhysicsPWindow::DisplayCollisionObjectOption(e_CollisionObjectType a_type, const Object* a_object) const
{
    switch (a_type)
    {
    case CollisionObjectType_Rigidbody:
    {
        return a_object->GetObjectType() != ObjectType_ArmatureNode;
    }
    case CollisionObjectType_Softbody:
    {
        switch (a_object->GetObjectType())
        {
        case ObjectType_CurveModel:
        case ObjectType_PathModel:
        {
            return true;
        }
        }

        return false;
    }
    }

    return true;
}

e_ObjectPropertiesTab PhysicsPWindow::GetWindowType() const
{
    return ObjectPropertiesTab_Physics;
}

void PhysicsPWindow::Update()
{
    constexpr float infinity = std::numeric_limits<float>::infinity();

    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const unsigned int objectCount = m_workspace->GetSelectedObjectCount();
        Object* const* objs = m_workspace->GetSelectedObjectArray();

        e_CollisionObjectType cOType = obj->GetCollisionObjectType();
        if (ImGui::BeginCombo("Physics Object Type", CollisionObjectString[cOType]))
        {
            for (int i = 0; i < CollisionObjectType_End; ++i)
            {
                e_CollisionObjectType type = (e_CollisionObjectType)i;
                if (DisplayCollisionObjectOption(type, obj) && ImGui::Selectable(CollisionObjectString[i]))
                {                    
                    m_workspace->PushActionSet(new SetCollisionObjectTypeAction(type, objs, objectCount, m_workspace, m_editor->GetPhysicsEngine()), &type, "Error setting collsion object type");
                }
            }

            ImGui::EndCombo();
        }
        
        cOType = obj->GetCollisionObjectType();
        if (cOType != CollisionObjectType_Null)
        {
            const CollisionObject* cObj = obj->GetCollisionObject();

            bool active = cObj->IsActive();
            if (ImGui::Checkbox("Active", &active))
            {
                m_workspace->PushActionSet(new SetCollisionObjectActiveAction(objs, objectCount, active), &active, "Error setting collision object state");
            }

            switch (cOType)
            {
            case CollisionObjectType_Rigidbody:
            {
                const Rigidbody* body = (Rigidbody*)cObj;

                float mass = body->GetMass();
                if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, infinity))
                {
                    m_workspace->PushActionSet(new SetRigidbodyMassAction(objs, objectCount, mass), &mass, "Error setting rigidbody mass");
                }
            }
            case CollisionObjectType_CollisionObject:
            {
                ImGui::Separator();

                break;
            }
            case CollisionObjectType_Softbody:
            {
                const Softbody* body = (Softbody*)cObj;
                
                float mass = body->GetMass();
                if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, infinity))
                {
                    m_workspace->PushActionSet(new SetSoftbodyMassAction(objs, objectCount, mass), &mass, "Error setting softbody mass");
                }

                ImGui::Separator();

                float dampening = body->GetDampening();
                if (ImGui::SliderFloat("Dampening", &dampening, 0.0f, 1.0f))
                {
                    m_workspace->PushActionSet(new SetSoftbodyDampeningAction(objs, objectCount, dampening), &dampening, "Error setting softbody dampening");
                }

                switch (obj->GetObjectType())
                {
                case ObjectType_PathModel:
                {
                    LineStiffness("Stiffness", objs, objectCount, body);
                    LineAngularStiffness("Angular Stiffness", objs, objectCount, body);
                    LineVolumeStiffness("Volume Stiffness", objs, objectCount, body);

                    break;
                }
                case ObjectType_CurveModel:
                {
                    LineStiffness("Line Stiffness", objs, objectCount, body);
                    LineAngularStiffness("Line Angular Stiffness", objs, objectCount, body);
                    LineVolumeStiffness("Line Volume Stiffness", objs, objectCount, body);

                    float stiffness = body->GetFaceStiffness();
                    if (ImGui::SliderFloat("Face Stiffness", &stiffness, 0.0f, 1.0f))
                    {
                        m_workspace->PushActionSet(new SetSoftbodyFaceStiffnessAction(objs, objectCount, stiffness), &stiffness, "Error setting softbody face stiffness");
                    }

                    float angularStiffness = body->GetFaceAngularStiffness();
                    if (ImGui::SliderFloat("Face Angular Stiffness", &angularStiffness, 0.0f, 1.0f))
                    {
                        m_workspace->PushActionSet(new SetSoftbodyFaceAngularStiffnessAction(objs, objectCount, angularStiffness), &angularStiffness, "Error setting softbody face angular stiffness");
                    }

                    float volumeStiffness = body->GetFaceVolumeStiffness();
                    if (ImGui::SliderFloat("Face Volume Stiffness", &volumeStiffness, 0.0f, 1.0f))
                    {
                        m_workspace->PushActionSet(new SetSoftbodyFaceVolumeStiffnessAction(objs, objectCount, volumeStiffness), &volumeStiffness, "Error setting softbody face volume stiffness");
                    }

                    break;
                }
                }
                break;
            }
            }

            cOType = obj->GetCollisionObjectType();

            switch (cOType)
            {
            case CollisionObjectType_CollisionObject:
            case CollisionObjectType_Rigidbody:
            {
                e_CollisionShapeType sType = obj->GetCollisionShapeType();
                if (ImGui::BeginCombo("Collision Shape Type", CollisionShapeString[sType]))
                {
                    for (int i = 0; i < CollisionShapeType_End; ++i)
                    {
                        if (ImGui::Selectable(CollisionShapeString[i]))
                        {
                            e_CollisionShapeType type = (e_CollisionShapeType)i;

                            m_workspace->PushActionSet(new SetCollisionShapeTypeAction((e_CollisionShapeType)i, objs, objectCount), &type, "Error setting collision shape type");
                        }
                    }

                    ImGui::EndCombo();
                }

                sType = obj->GetCollisionShapeType();
                switch (sType)
                {
                case CollisionShapeType_Box:
                {
                    const BoxCollisionShape* box = (BoxCollisionShape*)obj->GetCollisionShape();

                    glm::vec3 halfExtents = box->GetHalfExtents();
                    glm::vec3 extents = halfExtents * 2.0f;
                    if (ImGui::DragFloat3("Extents", (float*)&extents, 0.1f, 0.0f, infinity))
                    {
                        halfExtents = extents * 0.5f;

                        m_workspace->PushActionSet(new SetBoxCollisionShapeHalfExtentsAction(objs, objectCount, halfExtents), &halfExtents, "Error setting box extents");
                    }

                    break;
                }
                case CollisionShapeType_Capsule:
                {
                    const CapsuleCollisionShape* capsule = (CapsuleCollisionShape*)obj->GetCollisionShape();

                    float height = capsule->GetHeight();
                    if (ImGui::DragFloat("Height", &height, 0.1f, 0.0f, infinity))
                    {
                        m_workspace->PushActionSet(new SetCapsuleCollisionShapeHeightAction(objs, objectCount, height), &height, "Error setting capsule height");   
                    }

                    float radius = capsule->GetRadius();
                    if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, infinity))
                    {
                        m_workspace->PushActionSet(new SetCapsuleCollisionShapeRadiusAction(objs, objectCount, radius), &radius, "Error setting capsule radius");
                    }

                    break;
                }
                case CollisionShapeType_Plane:
                {
                    const PlaneCollisionShape* plane = (PlaneCollisionShape*)obj->GetCollisionShape();

                    glm::vec3 dir = plane->GetDirection();
                    if (ImGui::DragFloat3("Direction", (float*)&dir, 0.1f))
                    {
                        m_workspace->PushActionSet(new SetPlaneCollisionShapeDirectionAction(objs, objectCount, dir), &dir, "Error setting plane direction");
                    }

                    float distance = plane->GetDistance();
                    if (ImGui::DragFloat("Distance", &distance, 0.1f))
                    {
                        m_workspace->PushActionSet(new SetPlaneCollisionShapeDistanceAction(objs, objectCount, distance), &distance, "Error setting plane distance");
                    }

                    break;
                }
                case CollisionShapeType_Sphere:
                {
                    const SphereCollisionShape* sphere = (SphereCollisionShape*)obj->GetCollisionShape();

                    float radius = sphere->GetRadius();
                    if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, infinity))
                    {
                        m_workspace->PushActionSet(new SetSphereCollisionShapeRadiusAction(objs, objectCount, radius), &radius, "Error setting sphere radius");
                    }

                    break;
                }
                }

                break;
            }
            }
        }

        delete[] objs;
    }
}