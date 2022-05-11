#include "IO/ObjectSerializer.h"

#include "CurveModel.h"
#include "Datastore.h"
#include "EditorControls/Editor.h"
#include "IO/CollisionObjectSerializer.h"
#include "IO/CollisionShapeSerializer.h"
#include "IO/CurveModelSerializer.h"
#include "IO/PathModelSerializer.h"
#include "Object.h"
#include "PathModel.h"
#include "Transform.h"

void ObjectSerializer::WriteObj(std::ofstream* a_file, const Object* a_object, bool a_smartStep, int a_curveSteps, int a_pathSteps, int a_shapeSteps)
{
    const char* name = a_object->GetIDName();
    const unsigned int nameLen = (unsigned int)strlen(name);

    const CurveModel* curveModel = a_object->m_curveModel;
    if (curveModel != nullptr)
    {
        a_file->write("o ", 2);
        a_file->write(name, nameLen);

        a_file->write("\n", 1);

        CurveModelSerializer::WriteOBJ(a_file, curveModel, a_smartStep, a_curveSteps);
    }

    const PathModel* pathModel = a_object->m_pathModel;
    if (pathModel != nullptr)
    {
        a_file->write("o ", 2);
        a_file->write(name, nameLen);

        a_file->write("\n", 1);

        PathModelSerializer::WriteOBJ(a_file, pathModel, a_pathSteps, a_shapeSteps);
    }

    delete[] name;
}

tinyxml2::XMLElement* ObjectSerializer::GenerateColladaNodeElement(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_name, const char* a_type)
{
    tinyxml2::XMLElement* nodeElement = a_doc->NewElement("node");
    a_parent->InsertEndChild(nodeElement);
    nodeElement->SetAttribute("id", a_id);
    nodeElement->SetAttribute("name", a_name);
    nodeElement->SetAttribute("type", a_type);

    return nodeElement;
}
tinyxml2::XMLElement* ObjectSerializer::GenerateColladaGeometryElement(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_name)
{
    tinyxml2::XMLElement* geometryElement = a_doc->NewElement("geometry");
    a_parent->InsertEndChild(geometryElement);

    geometryElement->SetAttribute("id", a_id);
    geometryElement->SetAttribute("name", a_name);

    return geometryElement;
}
tinyxml2::XMLElement* ObjectSerializer::WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_geometryElement, tinyxml2::XMLElement* a_controllerElement, tinyxml2::XMLElement* a_parentElement, const Object* a_object, bool a_stepAdjust, int a_curveSteps, int a_pathSteps, int a_shapeSteps)
{
    const char* name = a_object->GetNameNoWhitespace();
    const char* idStr = a_object->GetIDName();

    tinyxml2::XMLElement* nodeElement = nullptr;

    switch (a_object->m_objectType)
    {
    case ObjectType_Armature:
    {
        nodeElement = GenerateColladaNodeElement(a_doc, a_parentElement, idStr, name, "NODE");

        break;
    }
    case ObjectType_ArmatureNode:
    {
        nodeElement = GenerateColladaNodeElement(a_doc, a_parentElement, idStr, name, "JOINT");

        break;
    }
    case ObjectType_CurveModel:
    {
        const CurveModel* model = a_object->m_curveModel;
        if (model != nullptr)
        {
            tinyxml2::XMLElement* geometryElement = GenerateColladaGeometryElement(a_doc, a_geometryElement, idStr, name);
            nodeElement = GenerateColladaNodeElement(a_doc, a_parentElement, idStr, name, "NODE");

            tinyxml2::XMLElement* controllerElement = nullptr;
            const Object* armObject = model->GetArmature();
            const std::string conStr = std::string(idStr) + "-controller"; 

            if (armObject != nullptr)
            {
                controllerElement = a_doc->NewElement("controller");

                controllerElement = a_doc->NewElement("controller");
                a_controllerElement->InsertEndChild(controllerElement);
                controllerElement->SetAttribute("id", conStr.c_str());
                controllerElement->SetAttribute("name", name);
            }

            char* outRoot;

            CurveModelSerializer::WriteCollada(a_doc, geometryElement, controllerElement, model, idStr, name, a_stepAdjust, a_curveSteps, &outRoot);

            if (armObject != nullptr)
            {
                tinyxml2::XMLElement* controllerInstanceElement = a_doc->NewElement("instance_controller");
                nodeElement->InsertEndChild(controllerInstanceElement);
                controllerInstanceElement->SetAttribute("url", ("#" + std::string(conStr)).c_str());
                
                tinyxml2::XMLElement* skeletonElement = a_doc->NewElement("skeleton");
                controllerInstanceElement->InsertEndChild(skeletonElement);
                skeletonElement->SetText(("#" + std::string(outRoot)).c_str());

                delete[] outRoot;
            }
            else
            {   
                tinyxml2::XMLElement* geometryInstanceElement = a_doc->NewElement("instance_geometry");
                nodeElement->InsertEndChild(geometryInstanceElement);
                geometryInstanceElement->SetAttribute("url", ("#" + std::string(idStr)).c_str());
            }
        }  

        break;
    }
    case ObjectType_PathModel:
    {
        const PathModel* model = a_object->m_pathModel;
        if (model != nullptr)
        {
            tinyxml2::XMLElement* geometryElement = GenerateColladaGeometryElement(a_doc, a_geometryElement, idStr, name);
            nodeElement = GenerateColladaNodeElement(a_doc, a_parentElement, idStr, name, "NODE");
            
            tinyxml2::XMLElement* controllerElement = nullptr;
            const Object* armObject = model->GetArmature();
            const std::string conStr = std::string(idStr) + "-controller"; 

            if (armObject != nullptr)
            {
                controllerElement = a_doc->NewElement("controller");

                controllerElement = a_doc->NewElement("controller");
                a_controllerElement->InsertEndChild(controllerElement);
                controllerElement->SetAttribute("id", conStr.c_str());
                controllerElement->SetAttribute("name", name);
            }

            char* outRoot;

            PathModelSerializer::WriteCollada(a_doc, geometryElement, controllerElement, model, idStr, name, a_pathSteps, a_shapeSteps, &outRoot);

            if (armObject != nullptr)
            {
                tinyxml2::XMLElement* controllerInstanceElement = a_doc->NewElement("instance_controller");
                nodeElement->InsertEndChild(controllerInstanceElement);
                controllerInstanceElement->SetAttribute("url", ("#" + std::string(conStr)).c_str());
                
                tinyxml2::XMLElement* skeletonElement = a_doc->NewElement("skeleton");
                controllerInstanceElement->InsertEndChild(skeletonElement);
                skeletonElement->SetText(("#" + std::string(outRoot)).c_str());

                delete[] outRoot;
            }
            else
            {   
                tinyxml2::XMLElement* geometryInstanceElement = a_doc->NewElement("instance_geometry");
                nodeElement->InsertEndChild(geometryInstanceElement);
                geometryInstanceElement->SetAttribute("url", ("#" + std::string(idStr)).c_str());
            }
        }

        break;
    }
    }

    if (nodeElement != nullptr)
    {
        const Transform* transform = a_object->m_transform;

        tinyxml2::XMLElement* translateElement = a_doc->NewElement("translate");
        nodeElement->InsertEndChild(translateElement);

        const glm::vec3 translation = transform->Translation();

        translateElement->SetText((std::to_string(translation.x) + " " + std::to_string(translation.y) + " " + std::to_string(translation.z)).c_str());

        tinyxml2::XMLElement* rotationElement = a_doc->NewElement("rotation");
        nodeElement->InsertEndChild(rotationElement);

        const glm::quat quaternion = transform->Quaternion();
        const glm::vec4 axisAngle = glm::vec4(glm::axis(quaternion), glm::degrees(glm::angle(quaternion)));

        rotationElement->SetText((std::to_string(axisAngle.x) + " " + std::to_string(axisAngle.y) + " " + std::to_string(axisAngle.z) + " " + std::to_string(axisAngle.w)).c_str());

        tinyxml2::XMLElement* scaleElement = a_doc->NewElement("scale");
        nodeElement->InsertEndChild(scaleElement);

        const glm::vec3 scale = transform->Scale();

        scaleElement->SetText((std::to_string(scale.x) + " " + std::to_string(scale.y) + " " + std::to_string(scale.z)).c_str());
    }

    delete[] name;
    delete[] idStr;

    return nodeElement;
}

void ObjectSerializer::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element, const Object* a_object)
{
    a_element->SetAttribute("ID", std::to_string(a_object->m_id).c_str());
    a_element->SetAttribute("Name", a_object->m_name);   

    const Transform* transform = a_object->m_transform;
    if (transform != nullptr)
    {
        transform->Serialize(a_doc, a_element);
    }

    const char* referencePath = a_object->m_referencePath;
    if (referencePath != nullptr)
    {
        tinyxml2::XMLElement* element = a_doc->NewElement("ReferenceImage");
        a_element->InsertEndChild(element);

        element->SetAttribute("Path", referencePath);
    }

    const CurveModel* curveModel = a_object->m_curveModel; 
    if (curveModel != nullptr)
    {
        CurveModelSerializer::Serialize(a_doc, a_element, curveModel);
    }
    const PathModel* pathModel = a_object->m_pathModel;
    if (pathModel != nullptr)
    {
        PathModelSerializer::Serialize(a_doc, a_element, pathModel);
    }

    CollisionObject* collisionObject = a_object->m_collisionObject;
    if (collisionObject != nullptr)
    {
        CollisionObjectSerializer::Serialize(a_doc, a_element, collisionObject);
    }
    CollisionShape* collisionShape = a_object->m_collisionShape;
    if (collisionShape != nullptr)
    {
        CollisionShapeSerializer::Serializer(a_doc, a_element, collisionShape);
    }
}
Object* ObjectSerializer::ParseData(Workspace* a_workspace, Editor* a_editor, const tinyxml2::XMLElement* a_element, Object* a_parent, std::list<ObjectBoneGroup>* a_boneGroups, std::unordered_map<long long, long long>* a_idMap)
{
    const tinyxml2::XMLAttribute* idAtt = a_element->FindAttribute("ID");
    const tinyxml2::XMLAttribute* nameAtt = a_element->FindAttribute("Name");

    Object* obj = new Object(nameAtt->Value());
    if (a_parent != nullptr)
    {
        obj->SetParent(a_parent);
    }

    a_idMap->emplace(std::stoll(idAtt->Value()), obj->m_id);

    if (strcmp(a_element->Value(), "ArmatureNode") == 0)
    {
        obj->m_objectType = ObjectType_ArmatureNode;
    }

    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();

        if (strcmp(str, "Object") == 0)
        {
            ParseData(a_workspace, a_editor, iter, obj, a_boneGroups, a_idMap);
        }
        else if (strcmp(str, "ArmatureNode") == 0)
        {
            obj->m_objectType = ObjectType_Armature;

            ParseData(a_workspace, a_editor, iter, obj, a_boneGroups, a_idMap);
        }
        else if (strcmp(str, "Transform") == 0)
        {
            obj->m_transform->ParseData(iter);
        }
        else if (strcmp(str, "ReferenceImage") == 0)
        {
            obj->m_objectType = ObjectType_ReferenceImage;

            const char* val = iter->Attribute("Path");

            if (val != nullptr)
            {
                const int size = strlen(val) + 1;

                obj->m_referencePath = new char[size];

                for (int i = 0; i < size; ++i)
                {
                    obj->m_referencePath[i] = val[i];
                }

                obj->m_referenceImage = Datastore::GetTexture(obj->m_referencePath);
            }
        }
        else if (strcmp(str, "CurveModel") == 0)
        {
            std::list<BoneGroup> bones;

            obj->m_objectType = ObjectType_CurveModel;

            obj->m_curveModel = CurveModelSerializer::ParseData(a_workspace, iter, &bones);

            if (bones.size() > 0)
            {
                ObjectBoneGroup boneGroup;
                boneGroup.ID = obj->m_id;
                boneGroup.Bones = bones;

                a_boneGroups->emplace_back(boneGroup);
            }
        }
        else if (strcmp(str, "PathModel") == 0)
        {
            std::list<BoneGroup> bones;

            obj->m_objectType = ObjectType_PathModel;

            obj->m_pathModel = PathModelSerializer::ParseData(a_workspace, iter, &bones);

            if (bones.size() > 0)
            {
                ObjectBoneGroup boneGroup;
                boneGroup.ID = obj->m_id;
                boneGroup.Bones = bones;

                a_boneGroups->emplace_back(boneGroup);
            }
        }
        else if (strcmp(str, "CollisionObject") == 0)
        {
            obj->m_collisionObject = CollisionObjectSerializer::ParseData(a_workspace, iter, obj, a_editor->GetPhysicsEngine());

            CollisionShape* shape = obj->m_collisionShape;
            if (shape != nullptr)
            {
                obj->m_collisionObject->SetCollisionShape(shape);
            }
        }
        else if (strcmp(str, "CollisionShape") == 0)
        {
            obj->m_collisionShape = CollisionShapeSerializer::ParseData(iter, obj);

            CollisionObject* cObj = obj->m_collisionObject;
            if (cObj != nullptr)
            {
                cObj->SetCollisionShape(obj->m_collisionShape);
            }
        }
        else
        {
            printf("Object::ParseData: Invalid Element: ");
            printf(str);
            printf("\n");
        }
    }

    return obj;
}
void ObjectSerializer::PostParseData(Object* a_object, const std::list<ObjectBoneGroup>& a_bones, const std::unordered_map<long long, long long>& a_idMap)
{
    const long long id = a_object->m_id;

    switch (a_object->m_objectType)
    {
    case ObjectType_CurveModel:
    {
        CurveModel* model = a_object->m_curveModel;
        if (model != nullptr)
        {
            for (auto iter = a_bones.begin(); iter != a_bones.end(); ++iter)
            {
                if (iter->ID == id)
                {
                    CurveModelSerializer::PostParseData(model, iter->Bones, a_idMap);
                
                    break;
                }
            }

            model->Triangulate();
        }

        break;
    }
    case ObjectType_PathModel:
    {
        PathModel* model = a_object->m_pathModel;
        if (model != nullptr)
        {
            for (auto iter = a_bones.begin(); iter != a_bones.end(); ++iter)
            {
                if (iter->ID == id)
                {
                    PathModelSerializer::PostParseData(model, iter->Bones, a_idMap);
                
                    break;
                }
            }

            model->Triangulate();
        }
    }
    }    
}