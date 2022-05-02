#pragma once 

#include <fstream>
#include <list>
#include <unordered_map>

#include "tinyxml2.h"

struct ObjectBoneGroup;

class Editor;
class Object;
class Workspace;

struct BoneGroup
{
    unsigned int Index;
    unsigned int ClusterIndex;
    long long ID;
    float Weight;
};

class ObjectSerializer
{
private:
    static tinyxml2::XMLElement* GenerateColladaNodeElement(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_name, const char* a_type);
    static tinyxml2::XMLElement* GenerateColladaGeometryElement(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_name);

protected:

public:
    ObjectSerializer() = delete;

    static void WriteObj(std::ofstream* a_file, const Object* a_object, bool a_smartStep, int a_curveSteps, int a_pathSteps, int a_shapeSteps);
    static tinyxml2::XMLElement* WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_geometryElement, tinyxml2::XMLElement* a_controllerElement, tinyxml2::XMLElement* a_parentElement, const Object* a_object, bool a_stepAdjust, int a_curveSteps, int a_pathSteps, int a_shapeSteps);

    static void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element, const Object* a_object);
    static Object* ParseData(Workspace* a_workspace, Editor* a_editor, const tinyxml2::XMLElement* a_element, Object* a_parent, std::list<ObjectBoneGroup>* a_boneGroups, std::unordered_map<long long, long long>* a_idMap);
    static void PostParseData(Object* a_object, const std::list<ObjectBoneGroup>& a_bones, const std::unordered_map<long long, long long>& a_idMap);
};