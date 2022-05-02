#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

struct CurveFace;
struct CurveNodeCluster;
struct PathLine;
struct PathNodeCluster;
struct ShapeLine;
struct ShapeNodeCluster;
struct Vertex;

class PrimitiveGenerator
{
private:

protected:

public:
    PrimitiveGenerator() = delete;

    static void CreateCylinder(Vertex** a_vertices, unsigned int* a_vertexCount, unsigned int** a_indices, unsigned int* a_indexCont, float a_radius, int a_steps, float a_height, const glm::vec3& a_dir);
    static void CreateCone(Vertex** a_vertices, unsigned int* a_vertexCount, unsigned int** a_indices, unsigned int* a_indexCont, float a_radius, int a_steps, float a_height, const glm::vec3& a_dir);
    static void CreateIcoSphere(Vertex** a_vertices, unsigned int* a_vertexCount, unsigned int** a_indices, unsigned int* a_indexCount, float a_radius, int a_steps);
    static void CreateTorus(Vertex** a_vertices, unsigned int* a_vertexCount, unsigned int** a_indices, unsigned int* a_indexCount, float a_outerRadius, int a_outerSteps, float a_innerRadius, int a_innerSteps, const glm::vec3& a_dir);

    static void CreateCurveTriangle(CurveNodeCluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);
    static void CreateCurvePlane(CurveNodeCluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);
    static void CreateCurveSphere(CurveNodeCluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);
    static void CreateCurveCube(CurveNodeCluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);

    static void CreatePathCylinder(PathNodeCluster** a_pathNodesPtr, unsigned int* a_pathNodeCount, PathLine** a_pathLinesPtr, unsigned int* a_pathLineCount, ShapeNodeCluster** a_shapeNodePtr, unsigned int* a_shapeNodeCount, ShapeLine** a_shapeLinesPtr, unsigned int* a_shapeLineCount);
    static void CreatePathCone(PathNodeCluster** a_pathNodesPtr, unsigned int* a_pathNodeCount, PathLine** a_pathLinesPtr, unsigned int* a_pathLineCount, ShapeNodeCluster** a_shapeNodePtr, unsigned int* a_shapeNodeCount, ShapeLine** a_shapeLinesPtr, unsigned int* a_shapeLineCount);
    static void CreatePathTorus(PathNodeCluster** a_pathNodesPtr, unsigned int* a_pathNodeCount, PathLine** a_pathLinesPtr, unsigned int* a_pathLineCount, ShapeNodeCluster** a_shapeNodePtr, unsigned int* a_shapeNodeCount, ShapeLine** a_shapeLinesPtr, unsigned int* a_shapeLineCount);
    static void CreatePathSpiral(PathNodeCluster** a_pathNodesPtr, unsigned int* a_pathNodeCount, PathLine** a_pathLinesPtr, unsigned int* a_pathLineCount, ShapeNodeCluster** a_shapeNodePtr, unsigned int* a_shapeNodeCount, ShapeLine** a_shapeLinesPtr, unsigned int* a_shapeLineCount);
};