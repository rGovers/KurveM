#include "IO/CollisionObjectSerializer.h"

#include "Object.h"
#include "Physics/CollisionObjects/Rigidbody.h"
#include "Physics/CollisionObjects/Softbody.h"
#include "PhysicsEngine.h"
#include "Workspace.h"

void CollisionObjectSerializer::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, CollisionObject* a_object)
{
    tinyxml2::XMLElement* rootElement = a_doc->NewElement("CollisionObject");
    a_parent->InsertEndChild(rootElement);

    const e_CollisionObjectType type = a_object->GetCollisionObjectType();
    rootElement->SetAttribute("Type", (int)type);

    switch (type)
    {
    case CollisionObjectType_Rigidbody:
    {
        const Rigidbody* rBody = (Rigidbody*)a_object;

        tinyxml2::XMLElement* massElement = a_doc->NewElement("Mass");
        rootElement->InsertEndChild(massElement);
        massElement->SetText(rBody->GetMass());

        break;
    }
    case CollisionObjectType_Softbody:
    {
        const Softbody* sBody = (Softbody*)a_object;

        tinyxml2::XMLElement* massElement = a_doc->NewElement("Mass");
        rootElement->InsertEndChild(massElement);
        massElement->SetText(sBody->GetMass());

        tinyxml2::XMLElement* dampeningElement = a_doc->NewElement("Dampening");
        rootElement->InsertEndChild(dampeningElement);
        dampeningElement->SetText(sBody->GetDampening());

        tinyxml2::XMLElement* lineStiffnessElement = a_doc->NewElement("LineStiffness");
        rootElement->InsertEndChild(lineStiffnessElement);
        lineStiffnessElement->SetText(sBody->GetLineStiffness());

        tinyxml2::XMLElement* lineAngularStiffnessElement = a_doc->NewElement("LineAngularStiffness");
        rootElement->InsertEndChild(lineAngularStiffnessElement);
        lineAngularStiffnessElement->SetText(sBody->GetLineAngularStiffness());

        tinyxml2::XMLElement* lineVolumeStiffnessElement = a_doc->NewElement("LineVolumeStiffness");
        rootElement->InsertEndChild(lineVolumeStiffnessElement);
        lineVolumeStiffnessElement->SetText(sBody->GetLineVolumeStiffness());

        tinyxml2::XMLElement* faceStiffnessElement = a_doc->NewElement("FaceStiffness");
        rootElement->InsertEndChild(faceStiffnessElement);
        faceStiffnessElement->SetText(sBody->GetFaceStiffness());

        tinyxml2::XMLElement* faceAngularStiffnessElement = a_doc->NewElement("FaceAngularStiffness");
        rootElement->InsertEndChild(faceAngularStiffnessElement);
        faceAngularStiffnessElement->SetText(sBody->GetFaceAngularStiffness());

        tinyxml2::XMLElement* faceVolumeStiffnessElement = a_doc->NewElement("FaceVolumeStiffness");
        rootElement->InsertEndChild(faceVolumeStiffnessElement);
        faceVolumeStiffnessElement->SetText(sBody->GetFaceVolumeStiffness());

        break;
    }
    }
}

CollisionObject* CollisionObjectSerializer::ParseData(Workspace* a_workspace, const tinyxml2::XMLElement* a_element, Object* a_object, PhysicsEngine* a_engine)
{
    const e_CollisionObjectType type = (e_CollisionObjectType)a_element->IntAttribute("Type");

    CollisionObject* obj = nullptr;

    switch (type)
    {
    case CollisionObjectType_CollisionObject:
    {
        obj = new CollisionObject(a_object, a_engine);

        break;
    }
    case CollisionObjectType_Rigidbody:
    {
        Rigidbody* body = new Rigidbody(a_object, a_engine);
        obj = body;

        for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
        {
            const char* str = iter->Value();
            if (strcmp(str, "Mass") == 0)
            {
                body->SetMass(iter->FloatText());
            }
            else
            {
                printf("CollisionObject::ParseData: Invalid Element: ");
                printf(str);
                printf("\n");
            }
        }

        break;
    }
    case CollisionObjectType_Softbody:
    {
        Softbody* body = new Softbody(a_object, a_workspace, a_engine);
        obj = body;

        for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
        {
            const char* str = iter->Value();
            if (strcmp(str, "Mass") == 0)
            {
                body->SetMass(iter->FloatText());
            }
            else if (strcmp(str, "Dampening") == 0)
            {
                body->SetDampening(iter->FloatText());
            }
            else if (strcmp(str, "LineStiffness") == 0)
            {
                body->SetLineStiffness(iter->FloatText());
            }
            else if (strcmp(str, "LineAngularStiffness") == 0)
            {
                body->SetLineAngularStiffness(iter->FloatText());
            }
            else if (strcmp(str, "LineVolumeStiffness") == 0)
            {
                body->SetLineVolumeStiffness(iter->FloatText());
            }
            else if (strcmp(str, "FaceStiffness") == 0)
            {
                body->SetFaceStiffness(iter->FloatText());
            }
            else if(strcmp(str, "FaceAngularStiffness") == 0)
            {
                body->SetFaceAngularStiffness(iter->FloatText());
            }
            else if(strcmp(str, "FaceVolumeStiffness") == 0)
            {
                body->SetFaceVolumeStiffness(iter->FloatText());
            }
            else
            {
                printf("CollisionObject::ParseData: Invalid Element: ");
                printf(str);
                printf("\n");
            }
        }

        break;
    }
    }

    return obj;
}