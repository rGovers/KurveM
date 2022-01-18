#pragma once

#include "BezierCurveNode2.h"
#include "BezierCurveNode3.h"
#include "tinyxml2.h"

#include <fstream>

struct Vertex;

class Model;
class Workspace;

struct PathNode
{
    glm::vec2 Scale;
    float Rotation;
    BezierCurveNode3 Node;

    PathNode() 
    {
        Scale = glm::vec2(1.0f);
        Rotation = 0.0f;
        Node = BezierCurveNode3(glm::vec3(0.0f), glm::vec3(0.0f));
    }
    PathNode(const glm::vec2& a_scale, float a_rotation, const BezierCurveNode3& a_node)
    {
        Scale = a_scale;
        Rotation = a_rotation;
        Node = a_node;
    }
    PathNode(const PathNode& a_other)
    {
        Scale = a_other.Scale;
        Rotation = a_other.Rotation;
        Node = a_other.Node;
    }
};

class PathModel
{
private:
    Workspace*        m_workspace;

    int               m_shapeSteps;
    int               m_pathSteps;

    unsigned int      m_shapeNodeCount;
    BezierCurveNode2* m_shapeNodes;
    unsigned int      m_shapeIndexCount;
    unsigned int*     m_shapeIndices;

    unsigned int      m_pathNodeCount;
    PathNode*         m_pathNodes;
    unsigned int      m_pathIndexCount;
    unsigned int*     m_pathIndices;

    Model*            m_model;
protected:

public:
    PathModel(Workspace* a_workspace);
    ~PathModel();

    inline int GetShapeSteps() const
    {
        return m_shapeSteps;
    }
    inline void SetShapeSteps(int a_value)
    {
        m_shapeSteps = a_value;
    }

    inline int GetPathSteps() const
    {
        return m_pathSteps;
    }
    inline void SetPathSteps(int a_value)
    {
        m_pathSteps = a_value;
    }

    inline unsigned int GetPathIndexCount() const
    {
        return m_pathIndexCount;
    }
    inline unsigned int GetPathIndex(unsigned int a_index) const
    {
        return m_pathIndices[a_index];
    }
    inline unsigned int* GetPathIndices() const
    {
        return m_pathIndices;
    }

    inline unsigned int GetPathNodeCount() const
    {
        return m_pathNodeCount;
    }
    inline PathNode GetNode(unsigned int a_index) const
    {
        return m_pathNodes[a_index];
    }
    inline PathNode* GetNodes() const
    {
        return m_pathNodes;
    }

    inline BezierCurveNode2* GetShapeNodes() const
    {
        return m_shapeNodes;
    }
    inline BezierCurveNode2 GetShapeNode(unsigned int a_index) const
    {
        return m_shapeNodes[a_index];
    }
    inline unsigned int GetShapeNodeCount() const
    {
        return m_shapeNodeCount;
    }
    inline unsigned int GetShapeIndexCount() const
    {
        return m_shapeIndexCount;
    }
    inline unsigned int GetShapeIndex(unsigned int a_index) const
    {
        return m_shapeIndices[a_index];
    }
    inline unsigned int* GetShapeIndices() const
    {
        return m_shapeIndices;
    }

    inline Model* GetDisplayModel() const
    {
        return m_model;
    }

    void GetModelData(int a_shapeSteps, int a_pathSteps, unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const;

    void SetModelData(const PathNode* a_nodes, unsigned int a_nodeCount, const unsigned int* a_nodeIndices, unsigned int a_nodeIndexCount, const BezierCurveNode2* a_shapeNodes, unsigned int a_shapeNodeCount, const unsigned int* a_shapeIndices, unsigned int a_shapeIndexCount);
    void PassModelData(PathNode* a_nodes, unsigned int a_nodeCount, unsigned int* a_nodeIndices, unsigned int a_nodeIndexCount, BezierCurveNode2* a_shapeNodes, unsigned int a_shapeNodeCount, unsigned int* a_shapeIndices, unsigned int a_shapeIndexCount);
    void Triangulate();

    void PreTriangulate(unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const;
    void PostTriangulate(unsigned int* a_indices, unsigned int a_indexCount, Vertex* a_vertices, unsigned int a_vertexCount);

    void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent) const;
    void ParseData(const tinyxml2::XMLElement* a_element);

    void WriteOBJ(std::ofstream* a_file, int a_pathSteps, int a_shapeSteps) const;
    void WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_name, int a_pathSteps, int a_shapeSteps) const;
};