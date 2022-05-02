#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <fstream>
#include <list>
#include <unordered_map>

#include "tinyxml2.h"

class Object;

struct Vertex;

class MeshExporter
{
private:

protected:

public:
    MeshExporter() = delete;

    static void SplitVertices(const Vertex* a_vertices, unsigned int a_vertexCount, glm::vec4** a_positions, unsigned int* a_posCount, std::unordered_map<unsigned int, unsigned int>* a_posMap, glm::vec3** a_normals, unsigned int* a_normalCount, std::unordered_map<unsigned int, unsigned int>* a_normMap, glm::vec2** a_uvs, unsigned int* a_uvCount, std::unordered_map<unsigned int, unsigned int>* a_uvMap, glm::vec4** a_bones = nullptr, glm::vec4** a_weights = nullptr);

    static void ExportOBJMesh(std::ofstream* a_file, const Vertex* a_vertices, unsigned int a_vertexCount, const unsigned int* a_indices, unsigned int a_indexCount);

    static void ExportColladaMesh(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const Vertex* a_vertices, unsigned int a_vertexCount, const unsigned int* a_indices, unsigned int a_indexCount, const char* a_name);
    static void ExportColladaMesh(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const glm::vec4* a_posVerts, unsigned int a_posVertCount, const glm::vec3* a_normVerts, unsigned int a_normVertCount, const glm::vec2* a_uvVerts, unsigned int a_uvVertCount, const glm::vec4* a_bones, const glm::vec4* a_weights, const std::unordered_map<unsigned int, unsigned int>& a_posMap, const std::unordered_map<unsigned int, unsigned int>& a_normMap, const std::unordered_map<unsigned int, unsigned int>& a_uvMap, const unsigned int* a_indices, unsigned int a_indexCount, const char* a_name);

    static void ExportColladaBones(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parentController, const glm::vec4* a_bones, const glm::vec4* a_weights, unsigned int a_vertCount, const char* a_parentID, const char* a_name, const std::list<Object*> a_armNodes, char** a_outRoot);
};
