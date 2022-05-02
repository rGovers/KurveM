#pragma once

#include "tinyxml2.h"

class CollisionObject;
class Object;
class PhysicsEngine;

class CollisionObjectSerializer
{
private:

protected:

public:
    CollisionObjectSerializer() = delete;

    static void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, CollisionObject* a_object);
    static CollisionObject* ParseData(const tinyxml2::XMLElement* a_element, Object* a_object, PhysicsEngine* a_engine);
};