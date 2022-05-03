#pragma once

#include "tinyxml2.h"

class CollisionShape;
class Object;

class CollisionShapeSerializer
{
private:

protected:

public:
    CollisionShapeSerializer() = delete;

    static void Serializer(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, CollisionShape* a_shape);
    static CollisionShape* ParseData(const tinyxml2::XMLElement* a_element, Object* a_object);
};