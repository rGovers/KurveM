#pragma once

#include <fstream>
#include <list>
#include <unordered_map>

#include "tinyxml2.h"

class PathModel;
class Workspace;

struct BoneGroup;

class PathModelSerializer
{
private:

protected:

public:
    PathModelSerializer() = delete;

    static void WriteOBJ(std::ofstream* a_file, const PathModel* a_model, int a_pathSteps, int a_shapeSteps);
    static void WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, tinyxml2::XMLElement* a_parentController, const PathModel* a_model, const char* a_parentID, const char* a_name, int a_pathSteps, int a_shapeSteps, char** a_outRoot);

    static void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const PathModel* a_model);
    static PathModel* ParseData(Workspace* a_workspace, const tinyxml2::XMLElement* a_element, std::list<BoneGroup>* a_outBones);
    static void PostParseData(PathModel* a_model, const std::list<BoneGroup>& a_bones, const std::unordered_map<long long, long long>& a_idMap);
};