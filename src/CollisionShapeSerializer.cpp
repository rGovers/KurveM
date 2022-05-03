#include "IO/CollisionShapeSerializer.h"

#include "IO/XMLIO.h"
#include "Physics/CollisionShapes/BoxCollisionShape.h"
#include "Physics/CollisionShapes/CapsuleCollisionShape.h"
#include "Physics/CollisionShapes/MeshCollisionShape.h"
#include "Physics/CollisionShapes/PlaneCollisionShape.h"
#include "Physics/CollisionShapes/SphereCollisionShape.h"

void CollisionShapeSerializer::Serializer(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, CollisionShape* a_shape)
{
    tinyxml2::XMLElement* rootElement = a_doc->NewElement("CollisionShape");
    a_parent->InsertEndChild(rootElement);

    const e_CollisionShapeType type = a_shape->GetShapeType();
    rootElement->SetAttribute("Type", (int)type);

    switch (type)
    {
    case CollisionShapeType_Box:
    {
        BoxCollisionShape* shape = (BoxCollisionShape*)a_shape;

        XMLIO::WriteVec3(a_doc, rootElement, "HalfExtents", shape->GetHalfExtents(), glm::vec3(1.0f));

        break;
    }
    case CollisionShapeType_Capsule:
    {
        CapsuleCollisionShape* shape = (CapsuleCollisionShape*)a_shape;

        tinyxml2::XMLElement* radiusElement = a_doc->NewElement("Radius");
        rootElement->InsertEndChild(radiusElement);

        radiusElement->SetText(shape->GetRadius());

        tinyxml2::XMLElement* heightElement = a_doc->NewElement("Height");
        rootElement->InsertEndChild(heightElement);

        heightElement->SetText(shape->GetHeight());

        break;
    }
    case CollisionShapeType_Mesh:
    {
        MeshCollisionShape* shape = (MeshCollisionShape*)a_shape;

        tinyxml2::XMLElement* stepElement = a_doc->NewElement("Steps");
        rootElement->InsertEndChild(stepElement);

        stepElement->SetText(shape->GetSteps());

        tinyxml2::XMLElement* secStepElement = a_doc->NewElement("SecSteps");
        rootElement->InsertEndChild(secStepElement);

        secStepElement->SetText(shape->GetSecSteps());

        break;
    }
    case CollisionShapeType_Plane:
    {
        PlaneCollisionShape* shape = (PlaneCollisionShape*)a_shape;

        XMLIO::WriteVec3(a_doc, rootElement, "Direction", shape->GetDirection());

        tinyxml2::XMLElement* distanceElement = a_doc->NewElement("Distance");
        rootElement->InsertEndChild(distanceElement);

        distanceElement->SetText(shape->GetDistance());

        break;
    }
    case CollisionShapeType_Sphere:
    {
        SphereCollisionShape* shape = (SphereCollisionShape*)a_shape;

        tinyxml2::XMLElement* radiusElement = a_doc->NewElement("Radius");
        rootElement->InsertEndChild(radiusElement);

        radiusElement->SetText(shape->GetRadius());

        break;
    }
    }
}
CollisionShape* CollisionShapeSerializer::ParseData(const tinyxml2::XMLElement* a_element, Object* a_object)
{
    constexpr glm::vec3 one = glm::vec3(1.0f);

    CollisionShape* shape = nullptr;

    const e_CollisionShapeType type = (e_CollisionShapeType)a_element->IntAttribute("Type");

    switch (type)
    {
    case CollisionShapeType_Box:
    {
        BoxCollisionShape* box = new BoxCollisionShape(one);
        shape = box;

        for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
        {
            const char* str = iter->Value();

            if (strcmp(str, "HalfExtents") == 0)
            {
                box->SetHalfExtents(XMLIO::GetVec3(iter, one));
            }
            else
            {
                printf("CollisionShape::ParseData: Invalid Element: ");
                printf(str);
                printf("\n");
            }
        }

        break;
    }
    case CollisionShapeType_Capsule:
    {
        CapsuleCollisionShape* capsule = new CapsuleCollisionShape(1.0f, 0.5f);
        shape = capsule;

        for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
        {
            const char* str = iter->Value();

            if (strcmp(str, "Height") == 0)
            {
                capsule->SetHeight(iter->FloatText());
            }
            else if (strcmp(str, "Radius") == 0)
            {
                capsule->SetRadius(iter->FloatText());
            }
            else
            {
                printf("CollisionShape::ParseData: Invalid Element: ");
                printf(str);
                printf("\n");
            }
        }

        break;
    }
    case CollisionShapeType_Mesh:
    {
        MeshCollisionShape* mesh = new MeshCollisionShape(a_object);
        shape = mesh;

        for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
        {
            const char* str = iter->Value();

            if (strcmp(str, "Steps") == 0)
            {
                mesh->SetSteps(iter->IntText());
            }
            else if (strcmp(str, "SecSteps") == 0)
            {
                mesh->SetSecSteps(iter->IntText());
            }
            else
            {
                printf("CollisionShape::ParseData: Invalid Element: ");
                printf(str);
                printf("\n");
            }
        }

        break;
    }
    case CollisionShapeType_Plane:
    {
        PlaneCollisionShape* plane = new PlaneCollisionShape(glm::vec3(0.0f, -1.0f, 0.0f), 0.0f);
        shape = plane;

        for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
        {
            const char* str = iter->Value();

            if (strcmp(str, "Direction") == 0)
            {
                plane->SetDirection(XMLIO::GetVec3(iter));
            }
            else if (strcmp(str, "Radius") == 0)
            {
                plane->SetDistance(iter->FloatText());
            }
            else
            {
                printf("CollisionShape::ParseData: Invalid Element: ");
                printf(str);
                printf("\n");
            }
        }

        break;
    }
    case CollisionShapeType_Sphere:
    {
        SphereCollisionShape* sphere = new SphereCollisionShape(1.0f);
        shape = sphere;

        for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
        {
            const char* str = iter->Value();

            if (strcmp(str, "Radius") == 0)
            {
                sphere->SetRadius(iter->FloatText());
            }
            else
            {
                printf("CollisionShape::ParseData: Invalid Element: ");
                printf(str);
                printf("\n");
            }
        }

        break;
    }
    }

    return shape;
}