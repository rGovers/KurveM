#pragma once

#include <fstream>
#include <list>
#include <unordered_map>

#include "tinyxml2.h"

class CurveModel;
class Workspace;

struct BoneGroup;

class CurveModelSerializer
{
private:

protected:

public:
    CurveModelSerializer() = delete;

    static void WriteOBJ(std::ofstream* a_file, const CurveModel* a_model, bool a_stepAdjust, int a_steps);
    static void WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, tinyxml2::XMLElement* a_parentController, const CurveModel* a_model, const char* a_parentID, const char* a_name, bool a_stepAdjust, int a_steps, char** a_outRoot);

    static void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const CurveModel* a_model);
    static CurveModel* ParseData(Workspace* a_workspace, const tinyxml2::XMLElement* a_element, std::list<BoneGroup>* a_outBones);
    static void PostParseData(CurveModel* a_model, const std::list<BoneGroup>& a_bones, const std::unordered_map<long long, long long>& a_idMap);
};