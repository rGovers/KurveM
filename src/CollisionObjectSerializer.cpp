#include "IO/CollisionObjectSerializer.h"

#include "Object.h"
#include "Physics/CollisionObjects/Rigidbody.h"
#include "Physics/CollisionObjects/Softbody.h"
#include "PhysicsEngine.h"

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

        break;
    }
    }
}

CollisionObject* CollisionObjectSerializer::ParseData(const tinyxml2::XMLElement* a_element, Object* a_object, PhysicsEngine* a_engine)
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
        Softbody* body = new Softbody(a_object, a_engine);
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
    }

    return obj;
}