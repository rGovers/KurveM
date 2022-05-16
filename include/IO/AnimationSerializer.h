#pragma once

#include <unordered_map>

#include "tinyxml2.h"

class Animation;
class Workspace;

class AnimationSerializer
{
private:
    static void CreateColladaSampler(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_element); 
    static void CreateColladaSource(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_index, const char* a_str, unsigned int a_count);
    static void CreateColladaChannel(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_source, const char* a_object, const char* a_index);

protected:

public:
    AnimationSerializer() = delete;

    static void WriteCollada(const Workspace* a_workspace, tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element, const Animation* a_animation);

    static void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element, const Animation* a_animation);
    static Animation* ParseData(const tinyxml2::XMLElement* a_element);
    static void PostParseData(Animation* a_animation, const std::unordered_map<long long, long long>& a_idMap);
};