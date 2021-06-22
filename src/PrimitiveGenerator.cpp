#include "PrimitiveGenerator.h"

void PrimitiveGenerator::CreateCurveSphere(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount)
{
    std::vector<Node3Cluster> nodes;
    std::vector<CurveFace> faces;

    // 0
    Node3Cluster cTp;
    cTp.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.0f)));
    cTp.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-0.5f, 1.0f, 0.0f)));
    cTp.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.5f)));
    cTp.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, -0.5f)));

    // 1
    Node3Cluster cFr;
    cFr.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.5f, 1.0f)));
    cFr.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -0.5f, 1.0f)));
    cFr.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.0f, 1.0f)));
    cFr.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.0f, 1.0f)));
            
    // 2
    Node3Cluster cBk;
    cBk.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.5f, -1.0f)));
    cBk.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -0.5f, -1.0f)));
    cBk.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.5f, 0.0f, -1.0f)));
    cBk.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(-0.5f, 0.0f, -1.0f)));

    // 3
    Node3Cluster cRg;
    cRg.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.0f)));
    cRg.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, -0.5f, 0.0f)));
    cRg.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.5f)));
    cRg.Nodes.emplace_back(BezierCurveNode3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, -0.5f)));

    // 4
    Node3Cluster cLf;
    cLf.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.5f, 0.0f)));
    cLf.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, -0.5f, 0.0f)));
    cLf.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.5f)));
    cLf.Nodes.emplace_back(BezierCurveNode3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, -0.5f)));

    // 5
    Node3Cluster cBt;
    cBt.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.5f)));
    cBt.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, -0.5f)));
    cBt.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.5f, -1.0f, 0.0f)));
    cBt.Nodes.emplace_back(BezierCurveNode3(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(-0.5f, -1.0f, 0.0f)));

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

    *a_nodeCount = nodes.size();
    *a_nodePtr = new Node3Cluster[*a_nodeCount];

    for (unsigned int i = 0; i < *a_nodeCount; ++i)
    {
        (*a_nodePtr)[i] = nodes[i];
    }

    *a_faceCount = faces.size();
    *a_facePtr = new CurveFace[*a_faceCount];

    for (unsigned int i = 0; i < *a_faceCount; ++i)
    {
        (*a_facePtr)[i] = faces[i];
    }
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

    *a_nodeCount = nodes.size();
    *a_nodePtr = new Node3Cluster[*a_nodeCount];

    for (unsigned int i = 0; i < *a_nodeCount; ++i)
    {
        (*a_nodePtr)[i] = nodes[i];
    }

    *a_faceCount = faces.size();
    *a_facePtr = new CurveFace[*a_faceCount];

    for (unsigned int i = 0; i < *a_faceCount; ++i)
    {
        (*a_facePtr)[i] = faces[i];
    }
}