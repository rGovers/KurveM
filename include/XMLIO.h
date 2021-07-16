#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>

#include "tinyxml2.h"

class XMLIO
{
private:

protected:

public:
    static void WriteVec2(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_name, const glm::vec2& a_value, const glm::vec2& a_default = glm::vec2(0));
    static void ReadVec2(const tinyxml2::XMLElement* a_element, glm::vec2* a_value);

    static void WriteVec3(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_name, const glm::vec3& a_value, const glm::vec3& a_default = glm::vec3(0));
    static void ReadVec3(const tinyxml2::XMLElement* a_element, glm::vec3* a_value);

    static void WriteQuat(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_name, const glm::quat& a_value);
    static void ReadQuat(const tinyxml2::XMLElement* a_element, glm::quat* a_value);
};