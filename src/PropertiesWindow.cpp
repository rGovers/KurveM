#include "Windows/PropertiesWindow.h"

#include "Actions/RenameObjectAction.h"
#include "Actions/RotateObjectAction.h"
#include "Actions/ScaleObjectAction.h"
#include "Actions/SetAnimationNodeAction.h"
#include "Actions/SetBoxCollisionShapeHalfExtentsAction.h"
#include "Actions/SetCapsuleCollisionShapeHeightAction.h"
#include "Actions/SetCapsuleCollisionShapeRadiusAction.h"
#include "Actions/SetCollisionObjectActiveAction.h"
#include "Actions/SetCollisionObjectTypeAction.h"
#include "Actions/SetCollisionShapeTypeAction.h"
#include "Actions/SetCurveArmatureAction.h"
#include "Actions/SetCurveSmartStepAction.h"
#include "Actions/SetCurveStepsAction.h"
#include "Actions/SetPathArmatureAction.h"
#include "Actions/SetPathPathStepsAction.h"
#include "Actions/SetPathShapeStepsAction.h"
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
#include "Actions/TranslateObjectAction.h"
#include "CurveModel.h"
#include "EditorControls/Editor.h"
#include "imgui.h"
#include "ImGuiExt.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Object.h"
#include "PathModel.h"
#include "Physics/CollisionObjects/Rigidbody.h"
#include "Physics/CollisionObjects/Softbody.h"
#include "Physics/CollisionShapes/BoxCollisionShape.h"
#include "Physics/CollisionShapes/CapsuleCollisionShape.h"
#include "Physics/CollisionShapes/PlaneCollisionShape.h"
#include "Physics/CollisionShapes/SphereCollisionShape.h"
#include "Transform.h"
#include "Workspace.h"

const char* RotationMode_String[] = { "Axis Angle", "Quaternion", "Euler Angle" };
const char* CollisionObject_String[] = { "Null", "Collision Object", "Rigidbody", "Softbody" };
const char* CollisionShape_String[] = { "Null", "Box", "Capsule", "Mesh", "Plane", "Sphere" };

#define ANIMATE_TOOLTIP "Contains object animation settings"
#define OBJECT_TOOLTIP "Contains object settings"
#define CURVE_TOOLTIP "Contains curve model settings"
#define PATH_TOOLTIP "Contains path model settings"
#define PHYSICS_TOOLTIP "Contains object physics settings"

PropertiesWindow::PropertiesWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_transform = nullptr;

    m_rotationMode = RotationMode_AxisAngle;
    m_propertiesMode = ObjectPropertiesTab_Object;

    m_lastObject = nullptr;
    m_lastTime = -1;
    m_node.Time = -1;
}
PropertiesWindow::~PropertiesWindow()
{

}

void PropertiesWindow::PushRotation(const glm::quat& a_quat)
{
    switch (m_workspace->GetCurrentActionType())
    {
    case ActionType_RotateObject:
    {
        RotateObjectAction* action = (RotateObjectAction*)m_workspace->GetCurrentActionType();

        action->SetRotation(a_quat);
        action->Execute();

        break;
    }
    default:
    {
        Object** objs = m_workspace->GetSelectedObjectArray();

        Action* action = new RotateObjectAction(a_quat, objs, m_workspace->GetSelectedObjectCount());
        if (!m_workspace->PushAction(action))
        {
            printf("Error rotating object \n");

            delete action;
        }

        delete[] objs;

        break;
    }
    }
}
void PropertiesWindow::PushAnimationNode(Animation* a_animation, Object* a_obj, const AnimationNode& a_node)
{
    switch (m_workspace->GetCurrentActionType())
    {
    case ActionType_SetAnimationNode:
    {
        SetAnimationNodeAction* action = (SetAnimationNodeAction*)m_workspace->GetCurrentAction();

        action->SetNode(a_node);
        action->Execute();

        break;
    }
    default:
    {
        Action* action = new SetAnimationNodeAction(a_animation, a_obj, a_node);
        if (!m_workspace->PushAction(action))
        {
            printf("Failed to set animation node \n");

            delete action;
        }
        else
        {
            m_workspace->SetCurrentAction(action);
        }

        break;
    }
    }
}

void PropertiesWindow::RotationModeDisplay()
{
    if (ImGui::BeginCombo("Rotation Mode", RotationMode_String[m_rotationMode]))
    {
        for (int i = 0; i < RotationMode_End; ++i)
        {
            const bool selected = m_rotationMode == i;
            if (ImGui::Selectable(RotationMode_String[i], selected))
            {
                m_rotationMode = (e_RotationMode)i;

                switch (m_workspace->GetCurrentActionType())
                {
                case ActionType_RotateObject:
                {
                    m_workspace->ClearCurrentAction();

                    break;
                }
                }

                m_node.Time = -1;
                m_transform = nullptr;
            }

            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
}

bool PropertiesWindow::DisplayCollisionObjectOption(e_CollisionObjectType a_type, const Object* a_object) const
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

void PropertiesWindow::PropertiesTabButton(const char* a_label, const char* a_path, e_ObjectPropertiesTab a_propertiesTab, const char* a_tooltip)
{
    if (ImGuiExt::ImageToggleButton(a_label, a_path, m_propertiesMode == a_propertiesTab, glm::vec2(16, 16)))
    {
        m_propertiesMode = a_propertiesTab;
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        ImGui::Text(a_label);

        ImGui::Separator();

        ImGui::Text(a_tooltip);

        ImGui::EndTooltip();
    }
}

int GetArmatures(std::vector<Object*>* a_items, long long a_id, Object* a_object)
{
    int index = 0;

    if (a_object->GetID() == a_id)
    {
        index = a_items->size();
    }

    const e_ObjectType objType = a_object->GetObjectType();
    if (objType == ObjectType_Armature)
    {
        a_items->emplace_back(a_object);
    }

    const std::list<Object*> children = a_object->GetChildren();

    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        const int otherIndex = GetArmatures(a_items, a_id, *iter);

        if (otherIndex != 0)
        {
            index = otherIndex;
        }
    }

    return index;
}

void PropertiesWindow::ObjectTab()
{
    Action* curAction = m_workspace->GetCurrentAction();

    Object** objs = m_workspace->GetSelectedObjectArray();

    Object* obj = m_workspace->GetSelectedObject();
    Transform* transform = obj->GetTransform();

    const unsigned int objectCount = m_workspace->GetSelectedObjectCount();

    if (objectCount == 1)
    {
        char* buff = new char[1024];
        const char* name = obj->GetName();

        if (name == nullptr)
        {
            name = "";
        }

        int size = strlen(name);

        for (int i = 0; i <= size; ++i)
        {
            buff[i] = name[i];
        }

        if (ImGui::InputText("Name", buff, 1000))
        {
            switch (m_workspace->GetCurrentActionType())
            {
            case ActionType_RenameObject:
            {
                RenameObjectAction* action = (RenameObjectAction*)curAction;

                action->SetNewName(buff);
                action->Execute();

                break;
            }
            default:
            {
                Action* action = new RenameObjectAction(name, buff, obj);

                if (!m_workspace->PushAction(action))
                {
                    printf("Error Renaming Object \n");

                    delete action;
                }
                else
                {
                    m_workspace->SetCurrentAction(action);
                }

                break;
            }
            }
        }

        ImGui::Separator();
    }

    if (m_transform != transform)
    {
        m_transform = transform;

        m_transformQuaternion = m_transform->Quaternion();
        m_transformAxisAngle = glm::vec4(glm::axis(m_transformQuaternion), glm::angle(m_transformQuaternion));
        m_transformEuler = glm::eulerAngles(m_transformQuaternion);
    }

    glm::vec3 translation = m_transform->Translation();
    if (ImGui::DragFloat3("Translation", (float*)&translation, 0.01f))
    {
        switch (m_workspace->GetCurrentActionType())
        {
        case ActionType_TranslateObject:
        {
            TranslateObjectAction* action = (TranslateObjectAction*)curAction;

            action->SetTranslation(translation);
            action->Execute();

            break;
        }
        default:
        {
            Action* action = new TranslateObjectAction(translation, objs, objectCount);
            if (!m_workspace->PushAction(action))
            {
                printf("Error Renaming Object \n");

                delete action;
            }
            else
            {
                m_workspace->SetCurrentAction(action);
            }

            break;
        }
        }
    }

    RotationModeDisplay();

    switch (m_rotationMode)
    {
    case RotationMode_AxisAngle:
    {
        if (ImGui::DragFloat4("Axis Angle", (float*)&m_transformAxisAngle, 0.01f))
        {
            const float len = glm::length(m_transformAxisAngle.xyz());
            glm::vec3 axis = m_transformAxisAngle.xyz() / len;
            if (len <= 0)
            {
                axis = glm::vec3(0, 1, 0);
            }

            PushRotation(glm::angleAxis(m_transformAxisAngle.w, axis));
        }

        break;
    }
    case RotationMode_Quaternion:
    {
        if (ImGui::DragFloat4("Quaternion", (float*)&m_transformQuaternion, 0.01f))
        {
            PushRotation(glm::normalize(m_transformQuaternion));
        }

        break;
    }
    case RotationMode_EulerAngle:
    {
        if (ImGui::DragFloat3("Euler Angle", (float*)&m_transformEuler, 0.01f))
        {
            PushRotation(glm::angleAxis(m_transformEuler.x, glm::vec3(1, 0, 0)) * glm::angleAxis(m_transformEuler.y, glm::vec3(0, 1, 0)) * glm::angleAxis(m_transformEuler.z, glm::vec3(0, 0, 1)));
        }

        break;
    }
    }

    glm::vec3 scale = transform->Scale();
    if (ImGui::DragFloat3("Scale", (float *)&scale, 0.01f))
    {
        switch (m_workspace->GetCurrentActionType())
        {
        case ActionType_ScaleObject:
        {
            ScaleObjectAction* action = (ScaleObjectAction*)curAction;

            action->SetScale(scale);
            action->Execute();

            break;
        }
        default:
        {
            Action* action = new ScaleObjectAction(scale, objs, objectCount);
            if (!m_workspace->PushAction(action))
            {
                printf("Error Renaming Object \n");

                delete action;
            }
            else
            {
                m_workspace->SetCurrentAction(action);
            }

            break;
        }
        }
    }

    delete[] objs;
}
void PropertiesWindow::AnimateTab()
{
    Animation* animation = m_workspace->GetCurrentAnimation();
    Object* obj = m_workspace->GetSelectedObject();
    const float time = m_editor->GetSelectedTime();

    if (animation != nullptr)
    {
        if (m_lastObject != obj || m_node.Time != -1 || m_lastTime != time)
        {
            m_node = animation->GetNode(obj, time);

            m_nodeQuaternion = m_node.Rotation;
            m_nodeAxisAngle = glm::vec4(glm::axis(m_nodeQuaternion), glm::angle(m_nodeQuaternion));
            m_nodeEuler = glm::eulerAngles(m_nodeQuaternion);

            m_lastTime = time;
            m_lastObject = obj;
        }

        AnimationNode node = animation->GetKeyNode(obj, (int)(time * animation->GetReferenceFramerate()));

        if (node.Time >= 0)
        {
            if (ImGui::DragFloat3("Translation", (float*)&node.Translation, 0.01f))
            {
                PushAnimationNode(animation, obj, node);
            }

            RotationModeDisplay();

            switch (m_rotationMode)
            {
            case RotationMode_AxisAngle:
            {
                if (ImGui::DragFloat4("Axis Angle", (float*)&m_nodeAxisAngle, 0.01f))
                {
                    const float len = glm::length(m_nodeAxisAngle.xyz());
                    glm::vec3 axis = m_nodeAxisAngle.xyz() / len;
                    if (len <= 0)
                    {
                        axis = glm::vec3(0.0f, 1.0f, 0.0f);
                    }

                    node.Rotation = glm::angleAxis(m_nodeAxisAngle.w, axis);

                    PushAnimationNode(animation, obj, node);
                }

                break;
            }
            case RotationMode_Quaternion:
            {
                if (ImGui::DragFloat4("Quaternion", (float*)&m_nodeQuaternion, 0.01f))
                {
                    node.Rotation = glm::normalize(m_nodeQuaternion);

                    PushAnimationNode(animation, obj, node);
                }

                break;
            }
            case RotationMode_EulerAngle:
            {
                if (ImGui::DragFloat3("Euler Angle", (float*)&m_nodeEuler, 0.01f))
                {
                    node.Rotation = glm::angleAxis(m_nodeEuler.x, glm::vec3(1, 0, 0)) * glm::angleAxis(m_nodeEuler.y, glm::vec3(0, 1, 0)) * glm::angleAxis(m_nodeEuler.z, glm::vec3(0, 0, 1));

                    PushAnimationNode(animation, obj, node);
                }

                break;
            }
            }

            if (ImGui::DragFloat3("Scale", (float*)&node.Scale, 0.01f))
            {
                PushAnimationNode(animation, obj, node);
            }
        }
    }
    
}
void PropertiesWindow::CurveTab()
{
    Object* obj = m_workspace->GetSelectedObject();

    CurveModel* model = obj->GetCurveModel();
    if (model != nullptr)
    {
        const unsigned int objectCount = m_workspace->GetSelectedObjectCount();
        Object** objs = m_workspace->GetSelectedObjectArray();

        Object** curveObjects = new Object*[objectCount];
        int curveObjectCount = 0;
        for (int i = 0; i < objectCount; ++i)
        {
            if (objs[i]->GetObjectType() == ObjectType_CurveModel)
            {
                curveObjects[curveObjectCount++] = objs[i];
            }
        }

        int triSteps = model->GetSteps();
        if (ImGui::InputInt("Curve Resolution", &triSteps))
        {
            const int steps = glm::max(triSteps, 1);

            switch (m_workspace->GetCurrentActionType())
            {
            case ActionType_SetCurveSteps:
            {
                SetCurveStepsAction* action = (SetCurveStepsAction*)m_workspace->GetCurrentAction();

                action->SetSteps(steps);
                action->Execute();

                break;
            }
            default:
            {
                Action *action = new SetCurveStepsAction(m_workspace, curveObjects, curveObjectCount, steps);
                if (!m_workspace->PushAction(action))
                {
                    printf("Error Setting Curve Steps \n");

                    delete action;
                }
                else
                {
                    m_workspace->SetCurrentAction(action);
                }

                break;
            }
            }
        }

        bool stepAdjust = model->IsStepAdjusted();
        if (ImGui::Checkbox("Smart Step", &stepAdjust))
        {
            switch (m_workspace->GetCurrentActionType())
            {
            case ActionType_SetCurveSmartStep:
            {
                SetCurveSmartStepAction* action = (SetCurveSmartStepAction*)m_workspace->GetCurrentAction();

                action->SetValue(stepAdjust);
                action->Execute();

                break;
            }
            default:
            {
                Action* action = new SetCurveSmartStepAction(m_workspace, curveObjects, curveObjectCount, stepAdjust);
                if (!m_workspace->PushAction(action))
                {
                    printf("Error Setting Curve Smart Step \n");

                    delete action;
                }
                else
                {
                    m_workspace->SetCurrentAction(action);
                }  

                break;
            }
            }
        }

        int index = 0;
        std::vector<Object*> items;

        items.emplace_back(nullptr);

        const long long id = model->GetArmatureID();

        const std::list<Object*> objects = m_workspace->GetObjectList();
        for (auto iter = objects.begin(); iter != objects.end(); ++iter)
        {
            const int otherIndex = GetArmatures(&items, id, *iter);
            if (otherIndex != 0)
            {
                index = otherIndex;
            }
        }

        const Object* obj = items[index];

        const char* name = "Null";

        if (obj != nullptr)
        {
            name = obj->GetName();
        }

        if (ImGui::BeginCombo("Armature", name))
        {
            const int size = items.size();

            for (int i = 0; i < size; ++i)
            {
                name = "Null";

                obj = items[i];
                if (obj != nullptr)
                {
                    name = obj->GetName();
                }

                const bool selected = i == index;
                if (ImGui::Selectable(name, selected))
                {
                    long long curID = -1;
                    if (obj != nullptr)
                    {
                        curID = obj->GetID();
                    }

                    switch (m_workspace->GetCurrentActionType())
                    {
                    case ActionType_SetCurveArmature:
                    {
                        SetCurveArmatureAction* action = (SetCurveArmatureAction*)m_workspace->GetCurrentAction();

                        action->SetID(curID);
                        action->Execute();

                        break;
                    }
                    default:
                    {
                        Action* action = new SetCurveArmatureAction(m_workspace, curveObjects, curveObjectCount, curID);
                        if (!m_workspace->PushAction(action))
                        {
                            printf("Error Setting Armature \n");

                            delete action;
                        }
                        else
                        {
                            m_workspace->SetCurrentAction(action);
                        }

                        break;
                    }
                    }
                }

                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        delete[] objs;
        delete[] curveObjects;
    }
}
void PropertiesWindow::PathTab()
{
    Object* obj = m_workspace->GetSelectedObject();

    if (obj != nullptr)
    {
        PathModel* model = obj->GetPathModel();
        if (model != nullptr)
        {
            const unsigned int objectCount = m_workspace->GetSelectedObjectCount();
            Object** objs = m_workspace->GetSelectedObjectArray();

            unsigned int pathObjectCount = 0;
            Object** pathObjs = new Object*[objectCount];
            for (unsigned int i = 0; i < objectCount; ++i)
            {
                if (objs[i]->GetObjectType() == ObjectType_PathModel)
                {
                    pathObjs[pathObjectCount++] = objs[i];
                }
            }
            
            int shapeSteps = model->GetShapeSteps();
            if (ImGui::InputInt("Shape Steps", &shapeSteps))
            {
                shapeSteps = glm::max(shapeSteps, 1);

                switch (m_workspace->GetCurrentActionType())
                {
                case ActionType_SetPathShapeSteps:
                {
                    SetPathShapeStepsAction* action = (SetPathShapeStepsAction*)m_workspace->GetCurrentAction();
                
                    action->SetSteps(shapeSteps);
                    action->Execute();

                    break;
                }
                default:
                {
                    Action* action = new SetPathShapeStepsAction(m_workspace, pathObjs, pathObjectCount, shapeSteps);
                    if (!m_workspace->PushAction(action))
                    {
                        printf("Error Setting Path Shape Steps \n");

                        delete action;
                    }
                    else
                    {
                        m_workspace->SetCurrentAction(action);
                    }

                    break;
                }
                }
            }

            int pathSteps = model->GetPathSteps();
            if (ImGui::InputInt("Path Steps", &pathSteps))
            {
                pathSteps = glm::max(pathSteps, 1);

                switch (m_workspace->GetCurrentActionType())
                {
                case ActionType_SetPathPathSteps:
                {
                    SetPathPathStepsAction* action = (SetPathPathStepsAction*)m_workspace->GetCurrentAction();
                
                    action->SetSteps(pathSteps);
                    action->Execute();

                    break;
                }
                default:
                {
                    Action* action = new SetPathPathStepsAction(m_workspace, pathObjs, pathObjectCount, pathSteps);
                    if (!m_workspace->PushAction(action))
                    {
                        printf("Error Setting Path Path Steps \n");

                        delete action;
                    }
                    else
                    {
                        m_workspace->SetCurrentAction(action);
                    }

                    break;
                }
                }
            }

            int index = 0;
            std::vector<Object*> items;

            items.emplace_back(nullptr);

            const long long id = model->GetArmatureID();

            const std::list<Object*> objects = m_workspace->GetObjectList();
            for (auto iter = objects.begin(); iter != objects.end(); ++iter)
            {
                const int otherIndex = GetArmatures(&items, id, *iter);
                if (otherIndex != 0)
                {
                    index = otherIndex;
                }
            }

            const Object* obj = items[index];

            const char* name = "Null";

            if (obj != nullptr)
            {
                name = obj->GetName();
            }

            if (ImGui::BeginCombo("Armature", name))
            {
                const int size = (int)items.size();

                for (int i = 0; i < size; ++i)
                {
                    name = "Null";

                    obj = items[i];
                    if (obj != nullptr)
                    {
                        name = obj->GetName();
                    }

                    if (ImGui::Selectable(name, i == index))
                    {
                        long long curID = -1;
                        if (obj != nullptr)
                        {
                            curID = obj->GetID();
                        }

                        if (m_workspace->GetCurrentActionType() == ActionType_SetPathArmature)
                        {
                            SetPathArmatureAction* action = (SetPathArmatureAction*)m_workspace->GetCurrentAction();

                            action->SetID(curID);
                            action->Execute();
                        }
                        else
                        {
                            Action* action = new SetPathArmatureAction(m_workspace, objs, objectCount, curID);
                            if (!m_workspace->PushAction(action))
                            {
                                printf("Error setting path armature \n");

                                delete action;
                            }
                            else
                            {
                                m_workspace->SetCurrentAction(action);
                            }
                        }
                    }
                }

                ImGui::EndCombo();   
            }

            delete[] objs;
            delete[] pathObjs;
        }
    }
}

void PropertiesWindow::LineStiffness(const char* a_displayName, Object* const* a_objs, unsigned int a_objectCount, const Softbody* a_body) const
{
    float stiffness = a_body->GetLineStiffness();
    if (ImGui::SliderFloat(a_displayName, &stiffness, 0.0f, 1.0f))
    {
        m_workspace->PushActionSet(new SetSoftbodyLineStiffnessAction(a_objs, a_objectCount, stiffness), &stiffness, "Error setting softbody line stiffness");
    }
}
void PropertiesWindow::LineAngularStiffness(const char* a_displayName, Object* const* a_objs, unsigned int a_objectCount, const Softbody* a_body) const
{
    float stiffness = a_body->GetLineAngularStiffness();
    if (ImGui::SliderFloat(a_displayName, &stiffness, 0.0f, 1.0f))
    {
        m_workspace->PushActionSet(new SetSoftbodyLineAngularStiffnessAction(a_objs, a_objectCount, stiffness), &stiffness, "Error setting softbody line angular stiffness");
    }
}
void PropertiesWindow::LineVolumeStiffness(const char* a_displayName, Object* const* a_objs, unsigned int a_objectCount, const Softbody* a_body) const
{
    float stiffness = a_body->GetLineVolumeStiffness();
    if (ImGui::SliderFloat(a_displayName, &stiffness, 0.0f, 1.0f))
    {
        m_workspace->PushActionSet(new SetSoftbodyLineVolumeStiffnessAction(a_objs, a_objectCount, stiffness), &stiffness, "Error setting softbody line volume stiffness");
    }
}
void PropertiesWindow::PhysicsTab()
{
    constexpr float infinity = std::numeric_limits<float>::infinity();

    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const unsigned int objectCount = m_workspace->GetSelectedObjectCount();
        Object* const* objs = m_workspace->GetSelectedObjectArray();

        e_CollisionObjectType cOType = obj->GetCollisionObjectType();
        if (ImGui::BeginCombo("Physics Object Type", CollisionObject_String[cOType]))
        {
            for (int i = 0; i < CollisionObjectType_End; ++i)
            {
                e_CollisionObjectType type = (e_CollisionObjectType)i;
                if (DisplayCollisionObjectOption(type, obj) && ImGui::Selectable(CollisionObject_String[i]))
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
                if (m_workspace->GetCurrentActionType() == ActionType_SetCollisionObjectActive)
                {
                    SetCollisionObjectActiveAction* action = (SetCollisionObjectActiveAction*)m_workspace->GetCurrentAction();
                    action->SetState(active);

                    action->Execute();
                }
                else
                {
                    m_workspace->PushActionSet(new SetCollisionObjectActiveAction(objs, objectCount, active), "Error setting collision object state");
                }
            }

            switch (cOType)
            {
            case CollisionObjectType_Rigidbody:
            {
                const Rigidbody* body = (Rigidbody*)cObj;

                float mass = body->GetMass();
                if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, infinity))
                {
                    if (m_workspace->GetCurrentActionType() == ActionType_SetRigidbodyMass)
                    {
                        SetRigidbodyMassAction* action = (SetRigidbodyMassAction*)m_workspace->GetCurrentAction();
                        action->SetMass(mass);

                        action->Execute();
                    }
                    else
                    {
                        m_workspace->PushActionSet(new SetRigidbodyMassAction(objs, objectCount, mass), "Error setting rigidbody mass");
                    }
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
                if (ImGui::BeginCombo("Collision Shape Type", CollisionShape_String[sType]))
                {
                    for (int i = 0; i < CollisionShapeType_End; ++i)
                    {
                        if (ImGui::Selectable(CollisionShape_String[i]))
                        {
                            if (m_workspace->GetCurrentActionType() == ActionType_SetCollisionShapeType)
                            {
                                SetCollisionShapeTypeAction* action = (SetCollisionShapeTypeAction*)m_workspace->GetCurrentAction();
                                action->SetType((e_CollisionShapeType)i);

                                action->Execute();
                            }
                            else
                            {
                                m_workspace->PushActionSet(new SetCollisionShapeTypeAction((e_CollisionShapeType)i, objs, objectCount), "Error setting collision shape type");
                            }
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

                    const glm::vec3 halfExtents = box->GetHalfExtents();
                    glm::vec3 extents = halfExtents * 2.0f;
                    if (ImGui::DragFloat3("Extents", (float*)&extents, 0.1f, 0.0f, infinity))
                    {
                        if (m_workspace->GetCurrentActionType() == ActionType_SetBoxCollisionShapeHalfExtents)
                        {
                            SetBoxCollisionShapeHalfExtentsAction* action = (SetBoxCollisionShapeHalfExtentsAction*)m_workspace->GetCurrentAction();
                            action->SetHalfExtents(extents * 0.5f);

                            action->Execute();
                        }
                        else
                        {
                            m_workspace->PushActionSet(new SetBoxCollisionShapeHalfExtentsAction(objs, objectCount, extents * 0.5f), "Error setting box extents");
                        }
                    }

                    break;
                }
                case CollisionShapeType_Capsule:
                {
                    const CapsuleCollisionShape* capsule = (CapsuleCollisionShape*)obj->GetCollisionShape();

                    float height = capsule->GetHeight();
                    if (ImGui::DragFloat("Height", &height, 0.1f, 0.0f, infinity))
                    {
                        if (m_workspace->GetCurrentActionType() == ActionType_SetCapsuleCollisionShapeHeight)
                        {
                            SetCapsuleCollisionShapeHeightAction* action = (SetCapsuleCollisionShapeHeightAction*)m_workspace->GetCurrentAction();
                            action->SetHeight(height);

                            action->Execute();
                        }
                        else
                        {
                            m_workspace->PushActionSet(new SetCapsuleCollisionShapeHeightAction(objs, objectCount, height), "Error setting capsule height");
                        }
                    }

                    float radius = capsule->GetRadius();
                    if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, infinity))
                    {
                        if (m_workspace->GetCurrentActionType() == ActionType_SetCapsuleCollisionShapeRadius)
                        {
                            SetCapsuleCollisionShapeRadiusAction* action = (SetCapsuleCollisionShapeRadiusAction*)m_workspace->GetCurrentAction();
                            action->SetRadius(radius);

                            action->Execute();
                        }
                        else
                        {
                            m_workspace->PushActionSet(new SetCapsuleCollisionShapeRadiusAction(objs, objectCount, radius), "Error setting capsule radius");
                        }
                    }

                    break;
                }
                case CollisionShapeType_Plane:
                {
                    const PlaneCollisionShape* plane = (PlaneCollisionShape*)obj->GetCollisionShape();

                    glm::vec3 dir = plane->GetDirection();
                    if (ImGui::DragFloat3("Direction", (float*)&dir, 0.1f))
                    {
                        if (m_workspace->GetCurrentActionType() == ActionType_SetPlaneCollisionShapeDirection)
                        {
                            SetPlaneCollisionShapeDirectionAction* action = (SetPlaneCollisionShapeDirectionAction*)m_workspace->GetCurrentAction();
                            action->SetDirection(dir);

                            action->Execute();
                        }
                        else
                        {
                            m_workspace->PushActionSet(new SetPlaneCollisionShapeDirectionAction(objs, objectCount, dir), "Error setting plane direction");
                        }
                    }

                    float distance = plane->GetDistance();
                    if (ImGui::DragFloat("Distance", &distance, 0.1f))
                    {
                        if (m_workspace->GetCurrentActionType() == ActionType_SetPlaneCollisionShapeDistance)
                        {
                            SetPlaneCollisionShapeDistanceAction* action = (SetPlaneCollisionShapeDistanceAction*)m_workspace->GetCurrentAction();
                            action->SetDistance(distance);

                            action->Execute();
                        }
                        else
                        {
                            m_workspace->PushActionSet(new SetPlaneCollisionShapeDistanceAction(objs, objectCount, distance), "Error setting plane distance");
                        }
                    }

                    break;
                }
                case CollisionShapeType_Sphere:
                {
                    const SphereCollisionShape* sphere = (SphereCollisionShape*)obj->GetCollisionShape();

                    float radius = sphere->GetRadius();
                    if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, infinity))
                    {
                        if (m_workspace->GetCurrentActionType() == ActionType_SetSphereCollisionShapeRadius)
                        {
                            SetSphereCollisionShapeRadiusAction* action = (SetSphereCollisionShapeRadiusAction*)m_workspace->GetCurrentAction();
                            action->SetRadius(radius);

                            action->Execute();
                        }
                        else
                        {
                            m_workspace->PushActionSet(new SetSphereCollisionShapeRadiusAction(objs, objectCount, radius), "Error setting sphere radius");
                        }
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

void PropertiesWindow::Update(double a_delta)
{
    if (ImGui::Begin("Properties"))
    {
        const int objectCount = m_workspace->GetSelectedObjectCount();

        if (objectCount > 0)
        {
            Object* obj = m_workspace->GetSelectedObject();
            Transform* transform = obj->GetTransform();

            ImGui::BeginGroup();

            const e_ObjectType objectType = obj->GetObjectType();

            PropertiesTabButton("Object", "Textures/PROPERTIES_OBJECT.png", ObjectPropertiesTab_Object, OBJECT_TOOLTIP);

            switch (objectType)
            {
            case ObjectType_ArmatureNode:
            {
                if (m_editor->GetEditorMode() == EditorMode_Animate)
                {
                    PropertiesTabButton("Animate", "Textures/PROPERTIES_ANIMATE.png", ObjectPropertiesTab_Animate, ANIMATE_TOOLTIP);
                }

                break;
            }
            case ObjectType_CurveModel:
            {
                PropertiesTabButton("Curve", "Textures/PROPERTIES_CURVE.png", ObjectPropertiesTab_Curve, CURVE_TOOLTIP);

                break;
            }
            case ObjectType_PathModel:
            {
                PropertiesTabButton("Curve", "Textures/PROPERTIES_PATH.png", ObjectPropertiesTab_Path, PATH_TOOLTIP);

                break;
            }
            }

            PropertiesTabButton("Physics", "Textures/PROPERTIES_PHYICS.png", ObjectPropertiesTab_Physics, PHYSICS_TOOLTIP);

            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();

            switch (m_propertiesMode)
            {
            case ObjectPropertiesTab_Animate:
            {
                AnimateTab();

                break;
            }
            case ObjectPropertiesTab_Curve:
            {
                CurveTab();

                break;
            }
            case ObjectPropertiesTab_Path:
            {
                PathTab();

                break;
            }
            case ObjectPropertiesTab_Object:
            {
                ObjectTab();

                break;
            }
            case ObjectPropertiesTab_Physics:
            {
                PhysicsTab();

                break;
            }
            }

            ImGui::EndGroup();
        }
    }
    
    ImGui::End();
}