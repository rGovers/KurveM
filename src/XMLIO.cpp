#include "XMLIO.h"

void XMLIO::WriteVec2(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_name, const glm::vec2& a_value, const glm::vec2& a_default)
{
    tinyxml2::XMLElement* element = a_doc->NewElement(a_name);
    a_parent->InsertEndChild(element);

    if (a_value.x != a_default.x)
    {
        tinyxml2::XMLElement* xElement = a_doc->NewElement("X");
        element->InsertEndChild(xElement);
        xElement->SetText(a_value.x);
    }
    
    if (a_value.y != a_default.y)
    {
        tinyxml2::XMLElement* yELement = a_doc->NewElement("Y");
        element->InsertEndChild(yELement);
        yELement->SetText(a_value.y);
    }
}
void XMLIO::ReadVec2(const tinyxml2::XMLElement* a_element, glm::vec2* a_value)
{
    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();
        if (strcmp(str, "X") == 0)
        {
            a_value->x = iter->FloatText();
        }
        else if (strcmp(str, "Y") == 0)
        {
            a_value->y = iter->FloatText();
        }
        else 
        {
            printf("XMLIO::ReadVec2: Invalid Element: ");
            printf(str);
            printf("\n");

        }
    }
}
glm::vec2 XMLIO::GetVec2(const tinyxml2::XMLElement* a_element, const glm::vec2& a_default)
{
    glm::vec2 val = a_default;

    ReadVec2(a_element, &val);

    return val;
}

void XMLIO::WriteVec3(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_name, const glm::vec3& a_value, const glm::vec3& a_default)
{
    tinyxml2::XMLElement* element = a_doc->NewElement(a_name);
    a_parent->InsertEndChild(element);

    if (a_value.x != a_default.x)
    {
        tinyxml2::XMLElement* xElement = a_doc->NewElement("X");
        element->InsertEndChild(xElement);
        xElement->SetText(a_value.x);
    }
    
    if (a_value.y != a_default.y)
    {
        tinyxml2::XMLElement* yELement = a_doc->NewElement("Y");
        element->InsertEndChild(yELement);
        yELement->SetText(a_value.y);
    }
    
    if (a_value.z != a_default.z)
    {
        tinyxml2::XMLElement* zElement = a_doc->NewElement("Z");
        element->InsertEndChild(zElement);
        zElement->SetText(a_value.z);
    }
}
void XMLIO::ReadVec3(const tinyxml2::XMLElement* a_element, glm::vec3* a_value)
{
    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();
        if (strcmp(str, "X") == 0)
        {
            a_value->x = iter->FloatText();
        }
        else if (strcmp(str, "Y") == 0)
        {
            a_value->y = iter->FloatText();
        }
        else if (strcmp(str, "Z") == 0)
        {
            a_value->z = iter->FloatText();
        }
        else 
        {
            printf("XMLIO::ReadVec3: Invalid Element: ");
            printf(str);
            printf("\n");

        }
    }
}
glm::vec3 XMLIO::GetVec3(const tinyxml2::XMLElement* a_element, const glm::vec3& a_default)
{
    glm::vec3 val = a_default;

    ReadVec3(a_element, &val);

    return val;
}

void XMLIO::WriteQuat(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_name, const glm::quat& a_value)
{
    tinyxml2::XMLElement* element = a_doc->NewElement(a_name);
    a_parent->InsertEndChild(element);

    tinyxml2::XMLElement* xElement = a_doc->NewElement("X");
    element->InsertEndChild(xElement);
    xElement->SetText(a_value.x);

    tinyxml2::XMLElement* yElement = a_doc->NewElement("Y");
    element->InsertEndChild(yElement);
    yElement->SetText(a_value.y);

    tinyxml2::XMLElement* zElement = a_doc->NewElement("Z");
    element->InsertEndChild(zElement);
    zElement->SetText(a_value.z);

    tinyxml2::XMLElement* wElement = a_doc->NewElement("W");
    element->InsertEndChild(wElement);
    wElement->SetText(a_value.w);
}
void XMLIO::ReadQuat(const tinyxml2::XMLElement* a_element, glm::quat* a_value)
{
    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();

        if (strcmp(str, "X") == 0)
        {
            a_value->x = iter->FloatText();
        }
        else if (strcmp(str, "Y") == 0)
        {
            a_value->y = iter->FloatText();
        }
        else if (strcmp(str, "Z") == 0)
        {
            a_value->z = iter->FloatText();
        }
        else if (strcmp(str, "W") == 0)
        {
            a_value->w = iter->FloatText();
        }
        else
        {
            printf("XMLIO::ReadQuat: Invalid Element: ");
            printf(str);
            printf("\n");
        }
    }

    *a_value = glm::normalize(*a_value);
}
glm::quat XMLIO::GetQuat(const tinyxml2::XMLElement* a_element)
{
    glm::quat qt = glm::identity<glm::quat>();

    ReadQuat(a_element, &qt);

    return qt;
}