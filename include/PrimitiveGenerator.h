#pragma once

#include "CurveModel.h"
#include "PathModel.h"

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

    static void CreateCurveTriangle(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);
    static void CreateCurvePlane(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);
    static void CreateCurveSphere(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);
    static void CreateCurveCube(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);

    static void CreatePathCylinder(PathNodeCluster** a_pathNodesPtr, unsigned int* a_pathNodeCount, PathLine** a_pathLinesPtr, unsigned int* a_pathLineCount, ShapeNodeCluster** a_shapeNodePtr, unsigned int* a_shapeNodeCount, ShapeLine** a_shapeLinesPtr, unsigned int* a_shapeLineCount);
    static void CreatePathCone(PathNodeCluster** a_pathNodesPtr, unsigned int* a_pathNodeCount, PathLine** a_pathLinesPtr, unsigned int* a_pathLineCount, ShapeNodeCluster** a_shapeNodePtr, unsigned int* a_shapeNodeCount, ShapeLine** a_shapeLinesPtr, unsigned int* a_shapeLineCount);
    static void CreatePathTorus(PathNodeCluster** a_pathNodesPtr, unsigned int* a_pathNodeCount, PathLine** a_pathLinesPtr, unsigned int* a_pathLineCount, ShapeNodeCluster** a_shapeNodePtr, unsigned int* a_shapeNodeCount, ShapeLine** a_shapeLinesPtr, unsigned int* a_shapeLineCount);
    static void CreatePathSpiral(PathNodeCluster** a_pathNodesPtr, unsigned int* a_pathNodeCount, PathLine** a_pathLinesPtr, unsigned int* a_pathLineCount, ShapeNodeCluster** a_shapeNodePtr, unsigned int* a_shapeNodeCount, ShapeLine** a_shapeLinesPtr, unsigned int* a_shapeLineCount);
};