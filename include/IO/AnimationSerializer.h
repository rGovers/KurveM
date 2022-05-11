#pragma once

#include <unordered_map>

#include "tinyxml2.h"

class Animation;

class AnimationSerializer
{
private:

protected:

public:
    AnimationSerializer() = delete;

    static void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element, const Animation* a_animation);
    static Animation* ParseData(const tinyxml2::XMLElement* a_element);
    static void PostParseData(Animation* a_animation, const std::unordered_map<long long, long long>& a_idMap);
};