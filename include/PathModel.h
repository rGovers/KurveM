#pragma once

#include <list>
#include <unordered_map>
#include <vector>

#include "BezierCurveNode2.h"
#include "BezierCurveNode3.h"
#include "EditorControls/Editor.h"

struct Vertex;

class Model;
class Object;
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

struct ShapeNodeCluster
{
    std::vector<BezierCurveNode2> Nodes;

    ShapeNodeCluster() {}
    ShapeNodeCluster(const BezierCurveNode2& a_node)
    {
        Nodes.emplace_back(a_node);
    }
};

struct PathNodeCluster
{
    std::vector<PathNode> Nodes;

    PathNodeCluster() { }
    PathNodeCluster(const BezierCurveNode3& a_node)
    {
        Nodes.emplace_back(PathNode(glm::vec2(1.0f), 0.0f, a_node));
    }
    PathNodeCluster(const PathNode& a_node)
    {
        Nodes.emplace_back(a_node);
    }
    PathNodeCluster(const PathNodeCluster& a_other)
    {
        Nodes = a_other.Nodes;
    }
};

struct PathLine
{
    unsigned int Index[2];
    unsigned char ClusterIndex[2];

    PathLine() {}
    PathLine(unsigned int a_indexA, unsigned int a_indexB, unsigned char a_clusterIndexA, unsigned char a_clusterIndexB)
    {
        Index[0] = a_indexA;
        Index[1] = a_indexB;

        ClusterIndex[0] = a_clusterIndexA;
        ClusterIndex[1] = a_clusterIndexB;
    }
};
struct ShapeLine
{
    unsigned int Index[2];
    unsigned char ClusterIndex[2];

    ShapeLine() {}
    ShapeLine(unsigned int a_indexA, unsigned int a_indexB, unsigned char a_clusterIndexA, unsigned char a_clusterIndexB)
    {
        Index[0] = a_indexA;
        Index[1] = a_indexB;

        ClusterIndex[0] = a_clusterIndexA;
        ClusterIndex[1] = a_clusterIndexB;
    }
};

class PathModel
{
private:
    friend class PathModelSerializer;

    Workspace*        m_workspace;

    long long         m_armature;

    int               m_shapeSteps;
    int               m_pathSteps;

    unsigned int      m_shapeNodeCount;
    ShapeNodeCluster* m_shapeNodes;
    unsigned int      m_shapeLineCount;
    ShapeLine*        m_shapeLines;

    unsigned int      m_pathNodeCount;
    PathNodeCluster*  m_pathNodes;
    unsigned int      m_pathLineCount;
    PathLine*         m_pathLines;

    Model*            m_model;

    void GetArmatureNodes(std::list<Object*>* a_list, Object* a_object) const;
    glm::vec3 GetMirrorMultiplier(e_MirrorMode a_mode) const;

    void GeneratePathStep(const PathLine& a_line, const std::vector<Vertex>& a_shapeVertices, float a_lerp, float a_nextLerp, const std::unordered_map<long long, unsigned int>& a_idMap, std::vector<Vertex>* a_vertices) const;

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

    inline unsigned int GetPathLineCount() const
    {
        return m_pathLineCount;
    }
    inline PathLine GetPathLineCount(unsigned int a_index) const
    {
        return m_pathLines[a_index];
    }
    inline PathLine* GetPathLines() const
    {
        return m_pathLines;
    }

    inline unsigned int GetPathNodeCount() const
    {
        return m_pathNodeCount;
    }
    inline PathNodeCluster GetPathNode(unsigned int a_index) const
    {
        return m_pathNodes[a_index];
    }
    inline PathNodeCluster* GetPathNodes() const
    {
        return m_pathNodes;
    }

    inline ShapeNodeCluster* GetShapeNodes() const
    {
        return m_shapeNodes;
    }
    inline ShapeNodeCluster GetShapeNode(unsigned int a_index) const
    {
        return m_shapeNodes[a_index];
    }
    inline unsigned int GetShapeNodeCount() const
    {
        return m_shapeNodeCount;
    }

    inline unsigned int GetShapeLineCount() const
    {
        return m_shapeLineCount;
    }
    inline ShapeLine GetShapeLine(unsigned int a_index) const
    {
        return m_shapeLines[a_index];
    }
    inline ShapeLine* GetShapeLines() const
    {
        return m_shapeLines;
    }

    inline Model* GetDisplayModel() const
    {
        return m_model;
    }

    void SetArmature(long long a_value);
    void SetArmature(const Object* a_value);
    Object* GetArmature() const;
    inline long long GetArmatureID() const
    {
        return m_armature;
    }
    std::list<Object*> GetArmatureNodes() const;
    unsigned int GetArmatureNodeCount() const;

    unsigned int* GetMirroredPathIndices(unsigned int a_index, e_MirrorMode a_mode) const;
    void GetMirroredPathHandle(unsigned int a_index, unsigned char a_nodeIndex, e_MirrorMode a_mode, unsigned int** a_outIndex, unsigned char** a_outNodeIndex) const;

    unsigned int* GetMirroredShapeIndices(unsigned int a_index, e_MirrorMode a_mode) const;
    void GetMirroredShapeHandle(unsigned int a_index, unsigned char a_nodeIndex, e_MirrorMode a_mode, unsigned int** a_outIndex, unsigned char** a_outNodeIndex) const;

    void EmplacePathNodes(const PathNodeCluster* a_nodes, unsigned int a_nodeCount);
    void DestroyPathNodes(unsigned int a_startIndex, unsigned int a_endIndex);

    void EmplacePathLine(const PathLine& a_line);
    void EmplacePathLines(const PathLine* a_lines, unsigned int a_lineCount);
    void DestroyPathLine(unsigned int a_index);
    void DestroyPathLines(unsigned int a_startIndex, unsigned int a_endIndex);

    void EmplaceShapeNodes(const ShapeNodeCluster* a_nodes, unsigned int a_nodeCount);
    void DestroyShapeNodes(unsigned int a_startIndex, unsigned int a_endIndex);

    void EmplaceShapeLines(const ShapeLine* a_lines, unsigned int a_lineCount);
    void DestroyShapeLines(unsigned int a_startIndex, unsigned int a_endIndex);

    void GetModelData(int a_shapeSteps, int a_pathSteps, unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const;

    void SetModelData(const PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, const PathLine* a_pathLines, unsigned int a_pathLineCount, const ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, const ShapeLine* a_shapeLines, unsigned int a_shapeLineCount);
    void PassModelData(PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, PathLine* a_pathLines, unsigned int a_pathLineCount, ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, ShapeLine* a_shapeLines, unsigned int a_shapeLineCount);

    void SetPathModelData(const PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, const PathLine* a_pathLines, unsigned int a_pathLineCount);
    void PassPathModelData(PathNodeCluster* a_pathNodes, unsigned int a_pathNodeCount, PathLine* a_pathLines, unsigned int a_pathLineCount);

    void SetShapeModelData(const ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, const ShapeLine* a_shapeLines, unsigned int a_shapeLineCount);
    void PassShapeModelData(ShapeNodeCluster* a_shapeNodes, unsigned int a_shapeNodeCount, ShapeLine* a_shapeLines, unsigned int a_shapeLineCount);

    void Triangulate();

    void PreTriangulate(unsigned int** a_indices, unsigned int* a_indexCount, Vertex** a_vertices, unsigned int* a_vertexCount) const;
    void PostTriangulate(unsigned int* a_indices, unsigned int a_indexCount, Vertex* a_vertices, unsigned int a_vertexCount);
};