#include "PrimitiveGenerator.h"

#include "Model.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

void GetData(const std::vector<Node3Cluster>& a_nodes, const std::vector<CurveFace>& a_faces, Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount)
{
    *a_nodeCount = a_nodes.size();
    *a_nodePtr = new Node3Cluster[*a_nodeCount];

    for (unsigned int i = 0; i < *a_nodeCount; ++i)
    {
        (*a_nodePtr)[i] = a_nodes[i];
    }

    *a_faceCount = a_faces.size();
    *a_facePtr = new CurveFace[*a_faceCount];

    for (unsigned int i = 0; i < *a_faceCount; ++i)
    {
        const CurveFace face = a_faces[i];

        (*a_facePtr)[i] = face;

        switch (face.FaceMode)
        {
        case FaceMode_3Point:
        {   
            for (int j = 0; j < 6; ++j)
            {
                ++(*a_nodePtr)[face.Index[j]].Nodes[face.ClusterIndex[j]].FaceCount;
            }

            break;
        }
        case FaceMode_4Point:
        {
            for (int j = 0; j < 8; ++j)
            {
                ++(*a_nodePtr)[face.Index[j]].Nodes[face.ClusterIndex[j]].FaceCount;
            }

            break;
        }
        } 
    }
}
void GetData(const std::vector<PathNode>& a_pathNodes, const std::vector<unsigned int>& a_pathIndices, 
    const std::vector<BezierCurveNode2>& a_shapeNodes, const std::vector<unsigned int>& a_shapeIndices,
    PathNode** a_nodePtr, unsigned int* a_nodeCount, unsigned int** a_nodeIndicesPtr, unsigned int* a_nodeIndexCount, 
    BezierCurveNode2** a_shapeNodePtr, unsigned int* a_shapeNodeCount, unsigned int** a_shapeIndicesPtr, unsigned int* a_shapeIndexCount)
{
    *a_nodeCount = a_pathNodes.size();
    *a_nodePtr = new PathNode[*a_nodeCount];

    for (unsigned int i = 0; i < *a_nodeCount; ++i)
    {
        (*a_nodePtr)[i] = a_pathNodes[i];
    }

    *a_nodeIndexCount = a_pathIndices.size();
    *a_nodeIndicesPtr = new unsigned int[*a_nodeIndexCount];

    for (unsigned int i = 0; i < *a_nodeIndexCount; ++i)
    {
        (*a_nodeIndicesPtr)[i] = a_pathIndices[i];
    }

    *a_shapeNodeCount = a_shapeNodes.size();
    *a_shapeNodePtr = new BezierCurveNode2[*a_shapeNodeCount];

    for (unsigned int i = 0; i < *a_shapeNodeCount; ++i)
    {
        (*a_shapeNodePtr)[i] = a_shapeNodes[i];
    }

    *a_shapeIndexCount = a_shapeIndices.size();
    *a_shapeIndicesPtr = new unsigned int[*a_shapeIndexCount];

    for (unsigned int i = 0; i < *a_shapeIndexCount; ++i)
    {
        (*a_shapeIndicesPtr)[i] = a_shapeIndices[i];
    }
}
void GetData(const std::vector<Vertex>& a_vertices, const std::vector<unsigned int> a_indices, Vertex** a_verticesPtr, unsigned int* a_vertexCount, unsigned int** a_indicesPtr, unsigned int* a_indexCount)
{
    *a_vertexCount = a_vertices.size();
    *a_verticesPtr = new Vertex[*a_vertexCount];

    for (unsigned int i = 0; i < *a_vertexCount; ++i)
    {
        (*a_verticesPtr)[i] = a_vertices[i];
    }

    *a_indexCount = a_indices.size();
    *a_indicesPtr = new unsigned int[*a_indexCount];

    for (unsigned int i = 0; i < *a_indexCount; ++i)
    {
        (*a_indicesPtr)[i] = a_indices[i];
    }
}

void PrimitiveGenerator::CreateCylinder(Vertex** a_vertices, unsigned int* a_vertexCount, unsigned int** a_indices, unsigned int* a_indexCont, float a_radius, int a_steps, float a_height, const glm::vec3& a_dir)
{
    const float halfHeight = a_height * 0.5f;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indicies;

    const glm::vec3 hDU = a_dir * halfHeight;
    const glm::vec3 hDD = a_dir * -halfHeight;

    constexpr float pi2 = glm::pi<float>() * 2;

    vertices.emplace_back(Vertex{ glm::vec4(hDU, 1), a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });
    vertices.emplace_back(Vertex{ glm::vec4(hDD, 1), -a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });

    const glm::vec4 vec = glm::vec4(0, 0, a_radius, 1);
    constexpr glm::vec3 f = glm::vec3(0, 0, 1);
    glm::quat q = glm::angleAxis(pi2, a_dir);
    
    glm::vec3 rVec = q * vec; 
    glm::vec3 normal = q * f;

    vertices.emplace_back(Vertex{ glm::vec4(hDU + rVec, 1), a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });    
    vertices.emplace_back(Vertex{ glm::vec4(hDU + rVec, 1), normal, glm::vec2(0), glm::vec4(0), glm::vec4(0) });    

    vertices.emplace_back(Vertex{ glm::vec4(hDD + rVec, 1), -a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });    
    vertices.emplace_back(Vertex{ glm::vec4(hDD + rVec, 1), normal, glm::vec2(0), glm::vec4(0), glm::vec4(0) });    

    for (int i = 1; i <= a_steps; ++i)
    {
        const float angle = (float)i / a_steps * pi2;

        q = glm::angleAxis(angle, a_dir);
        rVec = q * vec;
        normal = q * f;

        vertices.emplace_back(Vertex{ glm::vec4(hDU + rVec, 1), a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });    
        vertices.emplace_back(Vertex{ glm::vec4(hDU + rVec, 1), normal, glm::vec2(0), glm::vec4(0), glm::vec4(0) });    

        vertices.emplace_back(Vertex{ glm::vec4(hDD + rVec, 1), -a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });    
        vertices.emplace_back(Vertex{ glm::vec4(hDD + rVec, 1), normal, glm::vec2(0), glm::vec4(0), glm::vec4(0) });    

        indicies.emplace_back(0);
        indicies.emplace_back(2 + ((i - 1) * 4) + 0);
        indicies.emplace_back(2 + (i * 4) + 0);

        indicies.emplace_back(1);
        indicies.emplace_back(2 + (i * 4) + 2);
        indicies.emplace_back(2 + ((i - 1) * 4) + 2);

        indicies.emplace_back(2 + (i * 4) + 1);
        indicies.emplace_back(2 + (i * 4) + 3);
        indicies.emplace_back(2 + ((i - 1) * 4) + 3);
        indicies.emplace_back(2 + (i * 4) + 1);
        indicies.emplace_back(2 + ((i - 1) * 4) + 3);
        indicies.emplace_back(2 + ((i - 1) * 4) + 1);
    }

    GetData(vertices, indicies, a_vertices, a_vertexCount, a_indices, a_indexCont);
}
void PrimitiveGenerator::CreateCone(Vertex** a_vertices, unsigned int* a_vertexCount, unsigned int** a_indices, unsigned int* a_indexCount, float a_radius, int a_steps, float a_height, const glm::vec3& a_dir)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.emplace_back(Vertex{ glm::vec4(0, 0, 0, 1), -a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });
    vertices.emplace_back(Vertex{ glm::vec4(a_dir * a_height, 1), a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });

    const glm::vec4 vec = glm::vec4(0, 0, a_radius, 1);
    const glm::vec3 f = glm::vec3(0, 0, 1);
    glm::quat q = glm::angleAxis(glm::pi<float>() * 2, a_dir);

    glm::vec3 rVec = q * vec; 
    glm::vec3 normal = q * f;

    vertices.emplace_back(Vertex{ glm::vec4(rVec, 1), -a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });
    vertices.emplace_back(Vertex{ glm::vec4(rVec, 1), normal, glm::vec2(0), glm::vec4(0), glm::vec4(0) });

    for (int i = 1; i <= a_steps; ++i)
    {
        const float angle = (float)i / a_steps * (glm::pi<float>() * 2);

        q = glm::angleAxis(angle, a_dir);
        rVec = q * vec;
        normal = q * f;

        vertices.emplace_back(Vertex{ glm::vec4(rVec, 1), -a_dir, glm::vec2(0), glm::vec4(0), glm::vec4(0) });
        vertices.emplace_back(Vertex{ glm::vec4(rVec, 1), normal, glm::vec2(0), glm::vec4(0), glm::vec4(0) });

        indices.emplace_back(0);
        indices.emplace_back(2 + ((i - 1) * 2) + 0);
        indices.emplace_back(2 + (i * 2) + 0);

        indices.emplace_back(1);
        indices.emplace_back(2 + (i * 2) + 1);
        indices.emplace_back(2 + ((i - 1) * 2) + 1);
    }

    GetData(vertices, indices, a_vertices, a_vertexCount, a_indices, a_indexCount);
}
// Grabbed intial values from
// https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
// Quick and dirty not very well done
void PrimitiveGenerator::CreateIcoSphere(Vertex** a_vertices, unsigned int* a_vertexCount, unsigned** a_indices, unsigned int* a_indexCount, float a_radius, int a_steps)
{
    constexpr float x = 0.525731112119133606f;
    constexpr float z = 0.850650808352039932f;
    constexpr float n = 0.0f;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    vertices.emplace_back(glm::vec3(-x, n, z));
    vertices.emplace_back(glm::vec3(x, n, z));
    vertices.emplace_back(glm::vec3(-x, n, -z));
    vertices.emplace_back(glm::vec3(x, n, -z));

    vertices.emplace_back(glm::vec3(n, z, x));
    vertices.emplace_back(glm::vec3(n, z, -x));
    vertices.emplace_back(glm::vec3(n, -z, x));
    vertices.emplace_back(glm::vec3(n, -z, -x));

    vertices.emplace_back(glm::vec3(z, x, n));
    vertices.emplace_back(glm::vec3(-z, x, n));
    vertices.emplace_back(glm::vec3(z, -x, n));
    vertices.emplace_back(glm::vec3(-z, -x, n));

    indices.emplace_back(0); indices.emplace_back(4); indices.emplace_back(1);
    indices.emplace_back(0); indices.emplace_back(9); indices.emplace_back(4);
    indices.emplace_back(9); indices.emplace_back(5); indices.emplace_back(4);
    indices.emplace_back(4); indices.emplace_back(5); indices.emplace_back(8);
    indices.emplace_back(4); indices.emplace_back(8); indices.emplace_back(1);

    indices.emplace_back(8); indices.emplace_back(10); indices.emplace_back(1);
    indices.emplace_back(8); indices.emplace_back(3); indices.emplace_back(10);
    indices.emplace_back(5); indices.emplace_back(3); indices.emplace_back(8);
    indices.emplace_back(5); indices.emplace_back(2); indices.emplace_back(3);
    indices.emplace_back(2); indices.emplace_back(7); indices.emplace_back(3);

    indices.emplace_back(7); indices.emplace_back(10); indices.emplace_back(3);
    indices.emplace_back(7); indices.emplace_back(6); indices.emplace_back(10);
    indices.emplace_back(7); indices.emplace_back(11); indices.emplace_back(6);
    indices.emplace_back(11); indices.emplace_back(0); indices.emplace_back(6);
    indices.emplace_back(0); indices.emplace_back(1); indices.emplace_back(6);

    indices.emplace_back(6); indices.emplace_back(1); indices.emplace_back(10);
    indices.emplace_back(9); indices.emplace_back(0); indices.emplace_back(11);
    indices.emplace_back(9); indices.emplace_back(11); indices.emplace_back(2);
    indices.emplace_back(9); indices.emplace_back(2); indices.emplace_back(5);
    indices.emplace_back(7); indices.emplace_back(2); indices.emplace_back(11);

    for (int i = 0; i < a_steps; ++i)
    {
        std::vector<unsigned int> newIndices;

        for (unsigned int j = 0; j < indices.size(); j += 3)
        {
            const unsigned int indexA = indices[j + 0];
            const unsigned int indexB = indices[j + 1];
            const unsigned int indexC = indices[j + 2];

            const glm::vec3 vertA = vertices[indexA];
            const glm::vec3 vertB = vertices[indexB];
            const glm::vec3 vertC = vertices[indexC];

            const glm::vec3 midPosA = glm::normalize((vertA + vertB) * 0.5f);
            const glm::vec3 midPosB = glm::normalize((vertB + vertC) * 0.5f);
            const glm::vec3 midPosC = glm::normalize((vertC + vertA) * 0.5f);

            const unsigned int startIndex = vertices.size();

            vertices.emplace_back(midPosA);
            vertices.emplace_back(midPosB);
            vertices.emplace_back(midPosC);

            const unsigned int midIndexA = startIndex + 0;
            const unsigned int midIndexB = startIndex + 1;
            const unsigned int midIndexC = startIndex + 2;

            newIndices.emplace_back(indexA); newIndices.emplace_back(midIndexA); newIndices.emplace_back(midIndexC);
            newIndices.emplace_back(indexB); newIndices.emplace_back(midIndexB); newIndices.emplace_back(midIndexA);
            newIndices.emplace_back(indexC); newIndices.emplace_back(midIndexC); newIndices.emplace_back(midIndexB);
            newIndices.emplace_back(midIndexA); newIndices.emplace_back(midIndexB); newIndices.emplace_back(midIndexC);
        }   

        indices = newIndices;
    }
    
    std::vector<Vertex> fVertices;
    std::vector<unsigned int> fIndices;

    for (unsigned int i = 0; i < indices.size(); ++i)
    {
        const glm::vec3 norm = vertices[indices[i]]; 
        const glm::vec3 pos = norm * a_radius;

        for (unsigned int j = 0; j < fVertices.size(); ++j)
        {
            const glm::vec3 diff = pos - fVertices[j].Position.xyz();

            if (glm::dot(diff, diff) < 0.00001f)
            {
                fIndices.emplace_back(j);

                goto Next;
            }
        }

        fIndices.emplace_back(fVertices.size());
        fVertices.emplace_back(Vertex { glm::vec4(pos, 1), norm, glm::vec2(0), glm::vec4(0), glm::vec4(0) });
        
Next:;
    }

    GetData(fVertices, fIndices, a_vertices, a_vertexCount, a_indices, a_indexCount);
}

void CreateTorusInnerLoop(std::vector<Vertex>* a_vertices, float a_angle, int a_steps, float a_innerRadius, float a_outerRadius, const glm::vec3& a_dir)
{
    constexpr float pi2 = glm::pi<float>() * 2;

    const glm::quat q = glm::angleAxis(a_angle, a_dir);
    const glm::vec3 right = q * glm::vec3(1, 0, 0);

    const glm::vec3 f = q * glm::vec3(0, 0, a_outerRadius);

    for (int i = 1; i <= a_steps; ++i)
    {
        const float angle = (float)i / a_steps * pi2; 

        const glm::quat iQ = glm::angleAxis(angle, right);

        const glm::vec3 n = iQ * glm::vec3(0, 1, 0);

        a_vertices->emplace_back(Vertex { glm::vec4(f + (n * a_innerRadius), 1), n, glm::vec2(0), glm::vec4(0), glm::vec4(0) });
    }
}
void PrimitiveGenerator::CreateTorus(Vertex** a_vertices, unsigned int* a_vertexCount, unsigned int** a_indices, unsigned int* a_indexCount, float a_outerRadius, int a_outerSteps, float a_innerRadius, int a_innerSteps, const glm::vec3& a_dir)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    constexpr float pi2 = glm::pi<float>() * 2;

    CreateTorusInnerLoop(&vertices, pi2, a_innerSteps, a_innerRadius, a_outerRadius, a_dir);

    for (int i = 1; i <= a_outerSteps; ++i)
    {
        const float angle = (float)i / a_outerSteps * pi2;
        CreateTorusInnerLoop(&vertices, angle, a_innerSteps, a_innerRadius, a_outerRadius, a_dir);

        for (int j = 0; j < a_innerSteps; ++j)  
        {
            const unsigned int indexA = ((i - 1) * a_innerSteps) + (j + 0) % a_innerSteps;
            const unsigned int indexB = ((i - 1) * a_innerSteps) + (j + 1) % a_innerSteps;
            const unsigned int indexC = ((i - 0) * a_innerSteps) + (j + 0) % a_innerSteps;
            const unsigned int indexD = ((i - 0) * a_innerSteps) + (j + 1) % a_innerSteps;

            indices.emplace_back(indexA);
            indices.emplace_back(indexB);
            indices.emplace_back(indexC);

            indices.emplace_back(indexB);
            indices.emplace_back(indexD);
            indices.emplace_back(indexC);
        }
    }

    GetData(vertices, indices, a_vertices, a_vertexCount, a_indices, a_indexCount);
}

void PrimitiveGenerator::CreateCurveTriangle(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount)
{
    std::vector<Node3Cluster> nodes;
    std::vector<CurveFace> faces;

    Node3Cluster t;
    t.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f, 0)));
    t.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f, 0)));

    Node3Cluster bL;
    bL.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec2(0, 1)));
    bL.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec2(0, 1)));

    Node3Cluster bR;
    bR.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), glm::vec2(1, 1)));
    bR.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), glm::vec2(1, 1)));

    nodes.emplace_back(t);
    nodes.emplace_back(bL);
    nodes.emplace_back(bR);

    CurveFace face;

    face.FaceMode = FaceMode_3Point;
    
    face.Index[FaceIndex_3Point_AB] = 0;
    face.Index[FaceIndex_3Point_AC] = 0;
    face.Index[FaceIndex_3Point_BA] = 1;
    face.Index[FaceIndex_3Point_BC] = 1;
    face.Index[FaceIndex_3Point_CA] = 2;
    face.Index[FaceIndex_3Point_CB] = 2;

    face.ClusterIndex[FaceIndex_3Point_AB] = 0;
    face.ClusterIndex[FaceIndex_3Point_AC] = 1;
    face.ClusterIndex[FaceIndex_3Point_BA] = 0;
    face.ClusterIndex[FaceIndex_3Point_BC] = 1;
    face.ClusterIndex[FaceIndex_3Point_CA] = 0;
    face.ClusterIndex[FaceIndex_3Point_CB] = 1;

    faces.emplace_back(face);

    GetData(nodes, faces, a_nodePtr, a_nodeCount, a_facePtr, a_faceCount);
}
void PrimitiveGenerator::CreateCurvePlane(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount)
{
    std::vector<Node3Cluster> nodes;
    std::vector<CurveFace> faces;

    Node3Cluster tL;
    tL.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 1.0f), glm::vec2(0, 1)));
    tL.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 1.0f), glm::vec2(0, 1)));

    Node3Cluster tR;
    tR.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(1, 1)));
    tR.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(1, 1)));

    Node3Cluster bL;
    bL.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec2(0, 0)));
    bL.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec2(0, 0)));

    Node3Cluster bR;
    bR.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(1, 0)));
    bR.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(1, 0)));

    nodes.emplace_back(tL);
    nodes.emplace_back(tR);
    nodes.emplace_back(bL);
    nodes.emplace_back(bR);

    CurveFace face;

    face.FaceMode = FaceMode_4Point;

    face.Index[FaceIndex_4Point_AB] = 0;
    face.Index[FaceIndex_4Point_AC] = 0;
    face.Index[FaceIndex_4Point_BA] = 2;
    face.Index[FaceIndex_4Point_BD] = 2;
    face.Index[FaceIndex_4Point_CA] = 1;
    face.Index[FaceIndex_4Point_CD] = 1;
    face.Index[FaceIndex_4Point_DB] = 3;
    face.Index[FaceIndex_4Point_DC] = 3;

    face.ClusterIndex[FaceIndex_4Point_AB] = 0;
    face.ClusterIndex[FaceIndex_4Point_AC] = 1;
    face.ClusterIndex[FaceIndex_4Point_BA] = 0;
    face.ClusterIndex[FaceIndex_4Point_BD] = 1;
    face.ClusterIndex[FaceIndex_4Point_CA] = 0;
    face.ClusterIndex[FaceIndex_4Point_CD] = 1;
    face.ClusterIndex[FaceIndex_4Point_DB] = 0;
    face.ClusterIndex[FaceIndex_4Point_DC] = 1;

    faces.emplace_back(face);

    GetData(nodes, faces, a_nodePtr, a_nodeCount, a_facePtr, a_faceCount);
}
void PrimitiveGenerator::CreateCurveSphere(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount)
{
    std::vector<Node3Cluster> nodes;
    std::vector<CurveFace> faces;

    // 0
    Node3Cluster cTp;
    cTp.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.5f,  1.0f, 0.0f),  glm::vec2(0.5f, 0.0f)));
    cTp.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-0.5f, 1.0f, 0.0f),  glm::vec2(0.5f, 0.0f)));
    cTp.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f,  1.0f, 0.5f),  glm::vec2(0.5f, 0.0f)));
    cTp.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f,  1.0f, -0.5f), glm::vec2(0.5f, 0.0f)));

    // 1
    Node3Cluster cFr;
    cFr.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,  0.5f,  1.0f), glm::vec2(0, 0.5f)));
    cFr.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,  -0.5f, 1.0f), glm::vec2(0, 0.5f)));
    cFr.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.5f,  0.0f,  1.0f), glm::vec2(1, 0.5f)));
    cFr.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.0f,  1.0f), glm::vec2(0, 0.5f)));
            
    // 2
    Node3Cluster cBk;
    cBk.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f,  0.5f,  -1.0f), glm::vec2(0.5f, 0.5f)));
    cBk.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f,  -0.5f, -1.0f), glm::vec2(0.5f, 0.5f)));
    cBk.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.5f,  0.0f,  -1.0f), glm::vec2(0.5f, 0.5f)));
    cBk.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(-0.5f, 0.0f,  -1.0f), glm::vec2(0.5f, 0.5f)));

    // 3
    Node3Cluster cRg;
    cRg.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f,  0.0f),  glm::vec2(0.75f, 0.5f)));
    cRg.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, -0.5f, 0.0f),  glm::vec2(0.75f, 0.5f)));
    cRg.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f,  0.5f),  glm::vec2(0.75f, 0.5f)));
    cRg.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f,  -0.5f), glm::vec2(0.75f, 0.5f)));

    // 4
    Node3Cluster cLf;
    cLf.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.5f,  0.0f),  glm::vec2(0.25f, 0.5f)));
    cLf.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, -0.5f, 0.0f),  glm::vec2(0.25f, 0.5f)));
    cLf.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.5f),  glm::vec2(0.25f, 0.5f)));
    cLf.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  -0.5f), glm::vec2(0.25f, 0.5f)));

    // 5
    Node3Cluster cBt;
    cBt.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f,  -1.0f, 0.5f),  glm::vec2(0.5f, 1.0f)));
    cBt.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f,  -1.0f, -0.5f), glm::vec2(0.5f, 1.0f)));
    cBt.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.5f,  -1.0f, 0.0f),  glm::vec2(0.5f, 1.0f)));
    cBt.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(-0.5f, -1.0f, 0.0f),  glm::vec2(0.5f, 1.0f)));

    nodes.emplace_back(cTp);
    nodes.emplace_back(cFr);
    nodes.emplace_back(cBk);
    nodes.emplace_back(cRg);
    nodes.emplace_back(cLf);
    nodes.emplace_back(cBt);

    CurveFace tFRFace;

    tFRFace.FaceMode = FaceMode_3Point;

    // Top
    tFRFace.Index[FaceIndex_3Point_AB] = 0;
    tFRFace.Index[FaceIndex_3Point_AC] = 0;
    // Front
    tFRFace.Index[FaceIndex_3Point_BA] = 1;
    tFRFace.Index[FaceIndex_3Point_BC] = 1;
    // Right
    tFRFace.Index[FaceIndex_3Point_CA] = 3;
    tFRFace.Index[FaceIndex_3Point_CB] = 3;

    // Top - Front
    tFRFace.ClusterIndex[FaceIndex_3Point_AB] = 2;
    // Top - Right
    tFRFace.ClusterIndex[FaceIndex_3Point_AC] = 0;
    // Front - Top
    tFRFace.ClusterIndex[FaceIndex_3Point_BA] = 0;
    // Front - Right
    tFRFace.ClusterIndex[FaceIndex_3Point_BC] = 2;
    // Right - Top
    tFRFace.ClusterIndex[FaceIndex_3Point_CA] = 0;
    // Right - Front
    tFRFace.ClusterIndex[FaceIndex_3Point_CB] = 2;

    CurveFace tBRFace;

    tBRFace.FaceMode = FaceMode_3Point;

    // Top
    tBRFace.Index[FaceIndex_3Point_AB] = 0;
    tBRFace.Index[FaceIndex_3Point_AC] = 0;
    // Right
    tBRFace.Index[FaceIndex_3Point_BA] = 3;
    tBRFace.Index[FaceIndex_3Point_BC] = 3;
    // Back
    tBRFace.Index[FaceIndex_3Point_CA] = 2;
    tBRFace.Index[FaceIndex_3Point_CB] = 2;

    // Top - Right
    tBRFace.ClusterIndex[FaceIndex_3Point_AB] = 0;
    // Top - Back
    tBRFace.ClusterIndex[FaceIndex_3Point_AC] = 3;
    // Right - Top
    tBRFace.ClusterIndex[FaceIndex_3Point_BA] = 0;
    // Right - Back
    tBRFace.ClusterIndex[FaceIndex_3Point_BC] = 3;
    // Back - Top
    tBRFace.ClusterIndex[FaceIndex_3Point_CA] = 0;
    // Back - Right
    tBRFace.ClusterIndex[FaceIndex_3Point_CB] = 2;

    CurveFace tBLFace;

    tBLFace.FaceMode = FaceMode_3Point;

    // Top
    tBLFace.Index[FaceIndex_3Point_AB] = 0;
    tBLFace.Index[FaceIndex_3Point_AC] = 0;
    // Back
    tBLFace.Index[FaceIndex_3Point_BA] = 2;
    tBLFace.Index[FaceIndex_3Point_BC] = 2;
    // Left
    tBLFace.Index[FaceIndex_3Point_CA] = 4;
    tBLFace.Index[FaceIndex_3Point_CB] = 4;

    // Top - Back
    tBLFace.ClusterIndex[FaceIndex_3Point_AB] = 3;
    // Top - Left
    tBLFace.ClusterIndex[FaceIndex_3Point_AC] = 1;
    // Back - Top
    tBLFace.ClusterIndex[FaceIndex_3Point_BA] = 0;
    // Back - Left
    tBLFace.ClusterIndex[FaceIndex_3Point_BC] = 3;
    // Left - Top
    tBLFace.ClusterIndex[FaceIndex_3Point_CA] = 0;
    // Left - Back
    tBLFace.ClusterIndex[FaceIndex_3Point_CB] = 3;

    CurveFace tFLFace;

    tFLFace.FaceMode = FaceMode_3Point;

    // Top
    tFLFace.Index[FaceIndex_3Point_AB] = 0;
    tFLFace.Index[FaceIndex_3Point_AC] = 0;
    // Left
    tFLFace.Index[FaceIndex_3Point_BA] = 4;
    tFLFace.Index[FaceIndex_3Point_BC] = 4;
    // Front
    tFLFace.Index[FaceIndex_3Point_CA] = 1;
    tFLFace.Index[FaceIndex_3Point_CB] = 1;

    // Top - Left
    tFLFace.ClusterIndex[FaceIndex_3Point_AB] = 1;
    // Top - Front
    tFLFace.ClusterIndex[FaceIndex_3Point_AC] = 2;
    // Left - Top
    tFLFace.ClusterIndex[FaceIndex_3Point_BA] = 0;
    // Left - Front
    tFLFace.ClusterIndex[FaceIndex_3Point_BC] = 2;
    // Front - Top 
    tFLFace.ClusterIndex[FaceIndex_3Point_CA] = 0;
    // Front - Left
    tFLFace.ClusterIndex[FaceIndex_3Point_CB] = 3;

    CurveFace bFRFace;

    bFRFace.FaceMode = FaceMode_3Point;

    // Bottom
    bFRFace.Index[FaceIndex_3Point_AB] = 5;
    bFRFace.Index[FaceIndex_3Point_AC] = 5;
    // Right
    bFRFace.Index[FaceIndex_3Point_BA] = 3;
    bFRFace.Index[FaceIndex_3Point_BC] = 3;
    // Front
    bFRFace.Index[FaceIndex_3Point_CA] = 1;
    bFRFace.Index[FaceIndex_3Point_CB] = 1;

    // Bottom - Right
    bFRFace.ClusterIndex[FaceIndex_3Point_AB] = 2;
    // Bottom - Front
    bFRFace.ClusterIndex[FaceIndex_3Point_AC] = 0;
    // Right - Bottom
    bFRFace.ClusterIndex[FaceIndex_3Point_BA] = 1;
    // Right - Front
    bFRFace.ClusterIndex[FaceIndex_3Point_BC] = 2;
    // Front - Bottom
    bFRFace.ClusterIndex[FaceIndex_3Point_CA] = 1;
    // Front - Right
    bFRFace.ClusterIndex[FaceIndex_3Point_CB] = 2;

    CurveFace bBRFace;

    bBRFace.FaceMode = FaceMode_3Point;

    // Bottom
    bBRFace.Index[FaceIndex_3Point_AB] = 5;
    bBRFace.Index[FaceIndex_3Point_AC] = 5;
    // Back
    bBRFace.Index[FaceIndex_3Point_BA] = 2;
    bBRFace.Index[FaceIndex_3Point_BC] = 2;
    // Right
    bBRFace.Index[FaceIndex_3Point_CA] = 3;
    bBRFace.Index[FaceIndex_3Point_CB] = 3;

    // Bottom - Back
    bBRFace.ClusterIndex[FaceIndex_3Point_AB] = 1;
    // Bottom - Right
    bBRFace.ClusterIndex[FaceIndex_3Point_AC] = 2;
    // Back - Bottom
    bBRFace.ClusterIndex[FaceIndex_3Point_BA] = 1;
    // Back - Right
    bBRFace.ClusterIndex[FaceIndex_3Point_BC] = 2;
    // Right - Bottom
    bBRFace.ClusterIndex[FaceIndex_3Point_CA] = 1;
    // Right - Back
    bBRFace.ClusterIndex[FaceIndex_3Point_CB] = 3;

    CurveFace bBLFace;

    bBLFace.FaceMode = FaceMode_3Point;

    // Bottom
    bBLFace.Index[FaceIndex_3Point_AB] = 5;
    bBLFace.Index[FaceIndex_3Point_AC] = 5;
    // Left
    bBLFace.Index[FaceIndex_3Point_BA] = 4;
    bBLFace.Index[FaceIndex_3Point_BC] = 4;
    // Back
    bBLFace.Index[FaceIndex_3Point_CA] = 2;
    bBLFace.Index[FaceIndex_3Point_CB] = 2;

    // Bottom - Left
    bBLFace.ClusterIndex[FaceIndex_3Point_AB] = 3;
    // Bottom - Back
    bBLFace.ClusterIndex[FaceIndex_3Point_AC] = 1;
    // Left - Bottom
    bBLFace.ClusterIndex[FaceIndex_3Point_BA] = 1;
    // Left - Back
    bBLFace.ClusterIndex[FaceIndex_3Point_BC] = 3;
    // Back - Bottom
    bBLFace.ClusterIndex[FaceIndex_3Point_CA] = 1;
    // Back - Left
    bBLFace.ClusterIndex[FaceIndex_3Point_CB] = 3;

    CurveFace bFLFace;

    bFLFace.FaceMode = FaceMode_3Point;

    // Bottom
    bFLFace.Index[FaceIndex_3Point_AB] = 5;
    bFLFace.Index[FaceIndex_3Point_AC] = 5;
    // Front
    bFLFace.Index[FaceIndex_3Point_BA] = 1;
    bFLFace.Index[FaceIndex_3Point_BC] = 1;
    // Left
    bFLFace.Index[FaceIndex_3Point_CA] = 4;
    bFLFace.Index[FaceIndex_3Point_CB] = 4;

    // Bottom - Front
    bFLFace.ClusterIndex[FaceIndex_3Point_AB] = 0;
    // Bottom - Left
    bFLFace.ClusterIndex[FaceIndex_3Point_AC] = 3;
    // Front - Bottom
    bFLFace.ClusterIndex[FaceIndex_3Point_BA] = 1;
    // Front - Left
    bFLFace.ClusterIndex[FaceIndex_3Point_BC] = 3;
    // Left - Bottom
    bFLFace.ClusterIndex[FaceIndex_3Point_CA] = 1;
    // Left - Front
    bFLFace.ClusterIndex[FaceIndex_3Point_CB] = 2;

    faces.emplace_back(tFRFace);
    faces.emplace_back(tBRFace);
    faces.emplace_back(tBLFace);
    faces.emplace_back(tFLFace);
 
    faces.emplace_back(bFRFace);
    faces.emplace_back(bBRFace);
    faces.emplace_back(bBLFace);
    faces.emplace_back(bFLFace);

    GetData(nodes, faces, a_nodePtr, a_nodeCount, a_facePtr, a_faceCount);
}
void PrimitiveGenerator::CreateCurveCube(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount)
{   
    std::vector<Node3Cluster> nodes;
    std::vector<CurveFace> faces;

    // 0
    Node3Cluster cTpFrRg = Node3Cluster(BezierCurveNode3(glm::vec3(1.0f,  1.0f,  1.0f),  glm::vec3(1.0f,  1.0f,  1.0f)));
    // 1
    Node3Cluster cTpFrLf = Node3Cluster(BezierCurveNode3(glm::vec3(-1.0f, 1.0f,  1.0f),  glm::vec3(-1.0f, 1.0f,  1.0f)));
    // 2
    Node3Cluster cTpBkRg = Node3Cluster(BezierCurveNode3(glm::vec3(1.0f,  1.0f,  -1.0f), glm::vec3(1.0f,  1.0f,  -1.0f)));
    // 3
    Node3Cluster cTpBkLf = Node3Cluster(BezierCurveNode3(glm::vec3(-1.0f, 1.0f,  -1.0f), glm::vec3(-1.0f, 1.0f,  -1.0f)));
    // 4
    Node3Cluster cBtFrRg = Node3Cluster(BezierCurveNode3(glm::vec3(1.0f,  -1.0f, 1.0f),  glm::vec3(1.0f,  -1.0f, 1.0f)));
    // 5
    Node3Cluster cBtFrLf = Node3Cluster(BezierCurveNode3(glm::vec3(-1.0f, -1.0f, 1.0f),  glm::vec3(-1.0f, -1.0f, 1.0f)));
    // 6
    Node3Cluster cBtBkRg = Node3Cluster(BezierCurveNode3(glm::vec3(1.0f,  -1.0f, -1.0f), glm::vec3(1.0f,  -1.0f, -1.0f)));
    // 7
    Node3Cluster cBtBkLf = Node3Cluster(BezierCurveNode3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f)));

    nodes.emplace_back(cTpFrRg);
    nodes.emplace_back(cTpFrLf);
    nodes.emplace_back(cTpBkRg);
    nodes.emplace_back(cTpBkLf);
    nodes.emplace_back(cBtFrRg);
    nodes.emplace_back(cBtFrLf);
    nodes.emplace_back(cBtBkRg);
    nodes.emplace_back(cBtBkLf);

    CurveFace tpFace;

    tpFace.FaceMode = FaceMode_4Point;

    // Top Front Right
    tpFace.Index[FaceIndex_4Point_AB] = 0;
    tpFace.Index[FaceIndex_4Point_AC] = 0;
    // Top Back Right
    tpFace.Index[FaceIndex_4Point_BA] = 2;
    tpFace.Index[FaceIndex_4Point_BD] = 2;
    // Top Front Left
    tpFace.Index[FaceIndex_4Point_CA] = 1;
    tpFace.Index[FaceIndex_4Point_CD] = 1;
    // Top Back Left
    tpFace.Index[FaceIndex_4Point_DB] = 3;
    tpFace.Index[FaceIndex_4Point_DC] = 3;

    tpFace.ClusterIndex[FaceIndex_4Point_AB] = 0;
    tpFace.ClusterIndex[FaceIndex_4Point_AC] = 0;
    tpFace.ClusterIndex[FaceIndex_4Point_BA] = 0;
    tpFace.ClusterIndex[FaceIndex_4Point_BD] = 0;
    tpFace.ClusterIndex[FaceIndex_4Point_CA] = 0;
    tpFace.ClusterIndex[FaceIndex_4Point_CD] = 0;
    tpFace.ClusterIndex[FaceIndex_4Point_DB] = 0;
    tpFace.ClusterIndex[FaceIndex_4Point_DC] = 0;

    CurveFace frFace;

    frFace.FaceMode = FaceMode_4Point;

    // Top Front Right
    frFace.Index[FaceIndex_4Point_AB] = 0;
    frFace.Index[FaceIndex_4Point_AC] = 0;
    // Top Front Left
    frFace.Index[FaceIndex_4Point_BA] = 1;
    frFace.Index[FaceIndex_4Point_BD] = 1;
    // Bottom Front Right
    frFace.Index[FaceIndex_4Point_CA] = 4;
    frFace.Index[FaceIndex_4Point_CD] = 4;
    // Bottom Front Left
    frFace.Index[FaceIndex_4Point_DB] = 5;
    frFace.Index[FaceIndex_4Point_DC] = 5;

    frFace.ClusterIndex[FaceIndex_4Point_AB] = 0;
    frFace.ClusterIndex[FaceIndex_4Point_AC] = 0;
    frFace.ClusterIndex[FaceIndex_4Point_BA] = 0;
    frFace.ClusterIndex[FaceIndex_4Point_BD] = 0;
    frFace.ClusterIndex[FaceIndex_4Point_CA] = 0;
    frFace.ClusterIndex[FaceIndex_4Point_CD] = 0;
    frFace.ClusterIndex[FaceIndex_4Point_DB] = 0;
    frFace.ClusterIndex[FaceIndex_4Point_DC] = 0;

    CurveFace rgFace;

    rgFace.FaceMode = FaceMode_4Point;

    // Top Front Right
    rgFace.Index[FaceIndex_4Point_AB] = 0;
    rgFace.Index[FaceIndex_4Point_AC] = 0;
    // Bottom Front Right
    rgFace.Index[FaceIndex_4Point_BA] = 4;
    rgFace.Index[FaceIndex_4Point_BD] = 4;
    // Top Back Right
    rgFace.Index[FaceIndex_4Point_CA] = 2;
    rgFace.Index[FaceIndex_4Point_CD] = 2;
    // Bottom Back Right
    rgFace.Index[FaceIndex_4Point_DB] = 6;
    rgFace.Index[FaceIndex_4Point_DC] = 6;

    rgFace.ClusterIndex[FaceIndex_4Point_AB] = 0;
    rgFace.ClusterIndex[FaceIndex_4Point_AC] = 0;
    rgFace.ClusterIndex[FaceIndex_4Point_BA] = 0;
    rgFace.ClusterIndex[FaceIndex_4Point_BD] = 0;
    rgFace.ClusterIndex[FaceIndex_4Point_CA] = 0;
    rgFace.ClusterIndex[FaceIndex_4Point_CD] = 0;
    rgFace.ClusterIndex[FaceIndex_4Point_DB] = 0;
    rgFace.ClusterIndex[FaceIndex_4Point_DC] = 0;

    CurveFace bkFace;

    bkFace.FaceMode = FaceMode_4Point;

    // Top Back Left
    bkFace.Index[FaceIndex_4Point_AB] = 3;
    bkFace.Index[FaceIndex_4Point_AC] = 3;
    // Top Back Right
    bkFace.Index[FaceIndex_4Point_BA] = 2;
    bkFace.Index[FaceIndex_4Point_BD] = 2;
    // Bottom Back Left
    bkFace.Index[FaceIndex_4Point_CA] = 7;
    bkFace.Index[FaceIndex_4Point_CD] = 7;
    // Bottom Back Right
    bkFace.Index[FaceIndex_4Point_DB] = 6;
    bkFace.Index[FaceIndex_4Point_DC] = 6;

    bkFace.ClusterIndex[FaceIndex_4Point_AB] = 0;
    bkFace.ClusterIndex[FaceIndex_4Point_AC] = 0;
    bkFace.ClusterIndex[FaceIndex_4Point_BA] = 0;
    bkFace.ClusterIndex[FaceIndex_4Point_BD] = 0;
    bkFace.ClusterIndex[FaceIndex_4Point_CA] = 0;
    bkFace.ClusterIndex[FaceIndex_4Point_CD] = 0;
    bkFace.ClusterIndex[FaceIndex_4Point_DB] = 0;
    bkFace.ClusterIndex[FaceIndex_4Point_DC] = 0;

    CurveFace lfFace;

    lfFace.FaceMode = FaceMode_4Point;

    // Top Back Left
    lfFace.Index[FaceIndex_4Point_AB] = 3;
    lfFace.Index[FaceIndex_4Point_AC] = 3;
    // Bottom Back Left
    lfFace.Index[FaceIndex_4Point_BA] = 7;
    lfFace.Index[FaceIndex_4Point_BD] = 7;
    // Top Front Left
    lfFace.Index[FaceIndex_4Point_CA] = 1;
    lfFace.Index[FaceIndex_4Point_CD] = 1;
    // Bottom Front Left
    lfFace.Index[FaceIndex_4Point_DB] = 5;
    lfFace.Index[FaceIndex_4Point_DC] = 5;

    lfFace.ClusterIndex[FaceIndex_4Point_AB] = 0;
    lfFace.ClusterIndex[FaceIndex_4Point_AC] = 0;
    lfFace.ClusterIndex[FaceIndex_4Point_BA] = 0;
    lfFace.ClusterIndex[FaceIndex_4Point_BD] = 0;
    lfFace.ClusterIndex[FaceIndex_4Point_CA] = 0;
    lfFace.ClusterIndex[FaceIndex_4Point_CD] = 0;
    lfFace.ClusterIndex[FaceIndex_4Point_DB] = 0;
    lfFace.ClusterIndex[FaceIndex_4Point_DC] = 0;

    CurveFace btFace;

    btFace.FaceMode = FaceMode_4Point;

    // Bottom Back Left
    btFace.Index[FaceIndex_4Point_AB] = 7;
    btFace.Index[FaceIndex_4Point_AC] = 7;
    // Bottom Back Right
    btFace.Index[FaceIndex_4Point_BA] = 6;
    btFace.Index[FaceIndex_4Point_BD] = 6;
    // Bottom Front Left
    btFace.Index[FaceIndex_4Point_CA] = 5;
    btFace.Index[FaceIndex_4Point_CD] = 5;
    // Bottom Front Right
    btFace.Index[FaceIndex_4Point_DB] = 4;
    btFace.Index[FaceIndex_4Point_DC] = 4;

    btFace.ClusterIndex[FaceIndex_4Point_AB] = 0;
    btFace.ClusterIndex[FaceIndex_4Point_AC] = 0;
    btFace.ClusterIndex[FaceIndex_4Point_BA] = 0;
    btFace.ClusterIndex[FaceIndex_4Point_BD] = 0;
    btFace.ClusterIndex[FaceIndex_4Point_CA] = 0;
    btFace.ClusterIndex[FaceIndex_4Point_CD] = 0;
    btFace.ClusterIndex[FaceIndex_4Point_DB] = 0;
    btFace.ClusterIndex[FaceIndex_4Point_DC] = 0;

    faces.emplace_back(tpFace);
    faces.emplace_back(frFace);
    faces.emplace_back(rgFace);
    faces.emplace_back(bkFace);
    faces.emplace_back(lfFace);
    faces.emplace_back(btFace);

    GetData(nodes, faces, a_nodePtr, a_nodeCount, a_facePtr, a_faceCount);
}

void PrimitiveGenerator::CreatePathCylinder(PathNode** a_nodePtr, unsigned int* a_nodeCount, unsigned int** a_nodeIndicesPtr, unsigned int* a_nodeIndexCount, 
    BezierCurveNode2** a_shapeNodePtr, unsigned int* a_shapeNodeCount, unsigned int** a_shapeIndicesPtr, unsigned int* a_shapeIndexCount)
{
    std::vector<BezierCurveNode2> shapeNodes;
    std::vector<unsigned int> shapeIndices;

    std::vector<PathNode> pathNodes;
    std::vector<unsigned int> pathIndices;

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.5f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, -0.5f)));
    
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, 1.0f), glm::vec2(0.5f, 1.0f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, 1.0f), glm::vec2(-0.5f, 1.0f)));

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(-1.0f, 0.0f), glm::vec2(-1.0f, 0.5f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(-1.0f, 0.0f), glm::vec2(-1.0f, -0.5f)));

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, -1.0f), glm::vec2(0.5f, -1.0f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, -1.0f), glm::vec2(-0.5f, -1.0f)));

    shapeIndices.emplace_back(0); shapeIndices.emplace_back(2);
    shapeIndices.emplace_back(3); shapeIndices.emplace_back(4);
    shapeIndices.emplace_back(5); shapeIndices.emplace_back(7);
    shapeIndices.emplace_back(6); shapeIndices.emplace_back(1);

    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))));
    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))));

    pathIndices.emplace_back(0);
    pathIndices.emplace_back(1);

    GetData(pathNodes, pathIndices, shapeNodes, shapeIndices, a_nodePtr, a_nodeCount, a_nodeIndicesPtr, a_nodeIndexCount, a_shapeNodePtr, a_shapeNodeCount, a_shapeIndicesPtr, a_shapeIndexCount);
}
void PrimitiveGenerator::CreatePathCone(PathNode** a_nodePtr, unsigned int* a_nodeCount, unsigned int** a_nodeIndicesPtr, unsigned int* a_nodeIndexCount, 
    BezierCurveNode2** a_shapeNodePtr, unsigned int* a_shapeNodeCount, unsigned int** a_shapeIndicesPtr, unsigned int* a_shapeIndexCount)
{
    std::vector<BezierCurveNode2> shapeNodes;
    std::vector<unsigned int> shapeIndices;

    std::vector<PathNode> pathNodes;
    std::vector<unsigned int> pathIndices;

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.5f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, -0.5f)));
    
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, 1.0f), glm::vec2(0.5f, 1.0f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, 1.0f), glm::vec2(-0.5f, 1.0f)));

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(-1.0f, 0.0f), glm::vec2(-1.0f, 0.5f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(-1.0f, 0.0f), glm::vec2(-1.0f, -0.5f)));

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, -1.0f), glm::vec2(0.5f, -1.0f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, -1.0f), glm::vec2(-0.5f, -1.0f)));

    shapeIndices.emplace_back(0); shapeIndices.emplace_back(2);
    shapeIndices.emplace_back(3); shapeIndices.emplace_back(4);
    shapeIndices.emplace_back(5); shapeIndices.emplace_back(7);
    shapeIndices.emplace_back(6); shapeIndices.emplace_back(1);

    pathNodes.emplace_back(PathNode(glm::vec2(0.0f, 0.0f), 0.0f, BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))));
    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f))));

    pathIndices.emplace_back(0);
    pathIndices.emplace_back(1);

    GetData(pathNodes, pathIndices, shapeNodes, shapeIndices, a_nodePtr, a_nodeCount, a_nodeIndicesPtr, a_nodeIndexCount, a_shapeNodePtr, a_shapeNodeCount, a_shapeIndicesPtr, a_shapeIndexCount);
}
void PrimitiveGenerator::CreatePathTorus(PathNode** a_nodePtr, unsigned int* a_nodeCount, unsigned int** a_nodeIndicesPtr, unsigned int* a_nodeIndexCount, 
    BezierCurveNode2** a_shapeNodePtr, unsigned int* a_shapeNodeCount, unsigned int** a_shapeIndicesPtr, unsigned int* a_shapeIndexCount)
{
    std::vector<BezierCurveNode2> shapeNodes;
    std::vector<unsigned int> shapeIndices;

    std::vector<PathNode> pathNodes;
    std::vector<unsigned int> pathIndices;

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.5f, 0.0f), glm::vec2(0.5f, 0.25f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.5f, 0.0f), glm::vec2(0.5f, -0.25f)));

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, 0.5f), glm::vec2(0.25f, 0.5f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, 0.5f), glm::vec2(-0.25f, 0.5f)));   

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(-0.5f, 0.0f), glm::vec2(-0.5f, 0.25f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(-0.5f, 0.0f), glm::vec2(-0.5f, -0.25f)));

    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, -0.5f), glm::vec2(0.25f, -0.5f)));
    shapeNodes.emplace_back(BezierCurveNode2(glm::vec2(0.0f, -0.5f), glm::vec2(-0.25f, -0.5f)));   

    shapeIndices.emplace_back(0); shapeIndices.emplace_back(2);
    shapeIndices.emplace_back(3); shapeIndices.emplace_back(4);
    shapeIndices.emplace_back(5); shapeIndices.emplace_back(7);
    shapeIndices.emplace_back(6); shapeIndices.emplace_back(1);

    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.5f))));
    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, -0.5f))));

    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.0f, 1.0f))));
    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.0f, 1.0f))));

    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.5f))));
    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, -0.5f))));

    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.5f, 0.0f, -1.0f))));
    pathNodes.emplace_back(PathNode(glm::vec2(1.0f, 1.0f), 0.0f, BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(-0.5f, 0.0f, -1.0f))));

    pathIndices.emplace_back(0); pathIndices.emplace_back(2);
    pathIndices.emplace_back(3); pathIndices.emplace_back(4);
    pathIndices.emplace_back(5); pathIndices.emplace_back(7);
    pathIndices.emplace_back(6); pathIndices.emplace_back(1);

    GetData(pathNodes, pathIndices, shapeNodes, shapeIndices, a_nodePtr, a_nodeCount, a_nodeIndicesPtr, a_nodeIndexCount, a_shapeNodePtr, a_shapeNodeCount, a_shapeIndicesPtr, a_shapeIndexCount);
}