#include "Actions/InsertFaceAction.h"

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

void Wind3Points(Node3Cluster* a_nodes, unsigned int& a_a, unsigned int& a_b, unsigned int& a_c)
{
    const glm::vec2 posA = a_nodes[a_a].Nodes[0].Node.GetPosition();
    const glm::vec2 posB = a_nodes[a_b].Nodes[0].Node.GetPosition();
    const glm::vec2 posC = a_nodes[a_c].Nodes[0].Node.GetPosition();

    const glm::vec2 mid = (posA + posB + posC) * 0.33f;

    const glm::vec2 normA = glm::normalize(posA - mid);
    const glm::vec2 normB = glm::normalize(posB - mid);
    const glm::vec2 normC = glm::normalize(posC - mid);

    float angleA = atan2(normA.x, normA.y);
    float angleB = atan2(normB.x, normB.y);
    float angleC = atan2(normC.x, normC.y);

    if (angleB > angleC)
    {
        const unsigned int temp = a_b;
        a_b = a_c;
        a_c = temp;

        const float tempAngle = angleB;
        angleB = angleC;
        angleC = tempAngle;
    }

    if (angleA > angleB)
    {
        if (angleA > angleC)
        {
            const unsigned int temp = a_c;
            a_c = a_a;
            a_a = temp;
        }

        const unsigned int temp = a_b;
        a_b = a_a;
        a_a = temp;
    }
}

void Wind4Points(Node3Cluster* a_nodes, unsigned int& a_a, unsigned int& a_b, unsigned int& a_c, unsigned int& a_d)
{
    const glm::vec2 posA = a_nodes[a_a].Nodes[0].Node.GetPosition();
    const glm::vec2 posB = a_nodes[a_b].Nodes[0].Node.GetPosition();
    const glm::vec2 posC = a_nodes[a_c].Nodes[0].Node.GetPosition();
    const glm::vec2 posD = a_nodes[a_d].Nodes[0].Node.GetPosition();

    const glm::vec2 mid = (posA + posB + posC + posD) * 0.25f;

    const glm::vec2 normA = glm::normalize(posA - mid);
    const glm::vec2 normB = glm::normalize(posB - mid);
    const glm::vec2 normC = glm::normalize(posC - mid);
    const glm::vec2 normD = glm::normalize(posD - mid);

    float angleA = atan2(normA.x, normA.y);
    float angleB = atan2(normB.x, normB.y);
    float angleC = atan2(normC.x, normC.y);
    float angleD = atan2(normD.x, normD.y);

    if (angleC > angleD)
    {
        const unsigned int tmp = a_c;
        a_c = a_d;
        a_d = tmp;

        const float tmpAngle = angleC;
        angleC = angleD;
        angleD = tmpAngle;
    }

    if (angleB > angleC)
    {
        if (angleB > angleD)
        {
            const unsigned int tmp = a_b;
            a_b = a_d;
            a_d = tmp;

            const float tmpAngle = angleB;
            angleB = angleD;
            angleD = tmpAngle;
        }

        const unsigned int tmp = a_b;
        a_b = a_c;
        a_c = tmp;

        const float tmpAngle = angleB;
        angleB = angleC;
        angleC = tmpAngle;
    }

    if (angleA > angleB)
    {
        if (angleA > angleC)
        {
            if (angleA > angleD)
            {
                const unsigned int tmp = a_a;
                a_a = a_d;
                a_d = tmp;
            }

            const unsigned int tmp = a_a;
            a_a = a_c;
            a_c = tmp;
        }

        const unsigned int tmp = a_a;
        a_a = a_b;
        a_b = tmp;
    }
}

unsigned int PushNode(Node3Cluster* a_cluster)
{
    const unsigned int size = a_cluster->Nodes.size();

    for (unsigned int i = 0; i < size; ++i)
    {
        if (a_cluster->Nodes[i].Node.GetHandlePosition().x == std::numeric_limits<float>().infinity())
        {
            a_cluster->Nodes[i].Node.SetHandlePosition(a_cluster->Nodes[i].Node.GetPosition());

            return i;
        }
    }

    const glm::vec3 pos = a_cluster->Nodes[0].Node.GetPosition();

    BezierCurveNode3 node;
    node.SetPosition(pos);
    node.SetHandlePosition(pos);

    NodeGroup nodeG;
    nodeG.Node = node;
    nodeG.FaceCount = 1;

    a_cluster->Nodes.emplace_back(nodeG);

    return size;
}

void PopNode(Node3Cluster* a_cluster, const std::vector<NodeGroup>::iterator& a_iter)
{
    if (a_cluster->Nodes.size() > 1)
    {
        if (--a_iter->FaceCount <= 0)
        {
            a_cluster->Nodes.erase(a_iter);
        }
    }
    else if (a_cluster->Nodes.size() == 1)
    {
        if (--a_iter->FaceCount <= 0)
        {
            a_cluster->Nodes.begin()->Node.SetHandlePosition(glm::vec3(std::numeric_limits<float>().infinity()));
            a_cluster->Nodes.begin()->FaceCount;
        }
    }
}

InsertFaceAction::InsertFaceAction(Workspace* a_workspace, unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel)
{
    m_workspace = a_workspace;

    m_curveModel = a_curveModel;

    m_nodeCount = a_nodeCount;

    m_nodeIndices = new unsigned int[m_nodeCount];

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndices[i] = a_nodeIndices[i];
    }
}
InsertFaceAction::~InsertFaceAction()
{
    delete[] m_nodeIndices;
}

e_ActionType InsertFaceAction::GetActionType()
{
    return ActionType_InsertFace;
}

bool InsertFaceAction::Redo()
{
    return Execute();
}
bool InsertFaceAction::Execute() 
{
    Node3Cluster* nodes = m_curveModel->GetNodes();

    switch (m_nodeCount)
    {
    case 3:
    {
        unsigned int pointA = m_nodeIndices[0];
        unsigned int pointB = m_nodeIndices[1];
        unsigned int pointC = m_nodeIndices[2];
        
        Wind3Points(nodes, pointA, pointB, pointC);

        m_faceIndex = m_curveModel->GetFaceCount();

        CurveFace face;

        face.FaceMode = FaceMode_3Point;

        face.Index[FaceIndex_3Point_AB] = pointA;
        face.Index[FaceIndex_3Point_AC] = pointA;
        face.Index[FaceIndex_3Point_BA] = pointB;
        face.Index[FaceIndex_3Point_BC] = pointB;
        face.Index[FaceIndex_3Point_CA] = pointC;
        face.Index[FaceIndex_3Point_CB] = pointC;

        face.ClusterIndex[FaceIndex_3Point_AB] = PushNode(&nodes[pointA]);
        face.ClusterIndex[FaceIndex_3Point_AC] = PushNode(&nodes[pointA]);
        face.ClusterIndex[FaceIndex_3Point_BA] = PushNode(&nodes[pointB]);
        face.ClusterIndex[FaceIndex_3Point_BC] = PushNode(&nodes[pointB]);
        face.ClusterIndex[FaceIndex_3Point_CA] = PushNode(&nodes[pointC]);
        face.ClusterIndex[FaceIndex_3Point_CB] = PushNode(&nodes[pointC]);

        m_curveModel->EmplaceFace(face);

        m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

        return true;
    }
    case 4:
    {
        unsigned int pointA = m_nodeIndices[0];
        unsigned int pointB = m_nodeIndices[1];
        unsigned int pointC = m_nodeIndices[2];
        unsigned int pointD = m_nodeIndices[3];

        Wind4Points(nodes, pointA, pointB, pointC, pointD);

        m_faceIndex = m_curveModel->GetFaceCount();

        CurveFace face;

        face.FaceMode = FaceMode_4Point;

        face.Index[FaceIndex_4Point_AB] = pointA;
        face.Index[FaceIndex_4Point_AC] = pointA;
        face.Index[FaceIndex_4Point_BA] = pointB;
        face.Index[FaceIndex_4Point_BD] = pointB;
        face.Index[FaceIndex_4Point_CA] = pointC;
        face.Index[FaceIndex_4Point_CD] = pointC;
        face.Index[FaceIndex_4Point_DB] = pointD;
        face.Index[FaceIndex_4Point_DC] = pointD;

        face.ClusterIndex[FaceIndex_4Point_AB] = PushNode(&nodes[pointA]);
        face.ClusterIndex[FaceIndex_4Point_AC] = PushNode(&nodes[pointA]);
        face.ClusterIndex[FaceIndex_4Point_BA] = PushNode(&nodes[pointB]);
        face.ClusterIndex[FaceIndex_4Point_BD] = PushNode(&nodes[pointB]);
        face.ClusterIndex[FaceIndex_4Point_CA] = PushNode(&nodes[pointC]);
        face.ClusterIndex[FaceIndex_4Point_CD] = PushNode(&nodes[pointC]);
        face.ClusterIndex[FaceIndex_4Point_DB] = PushNode(&nodes[pointD]);
        face.ClusterIndex[FaceIndex_4Point_DC] = PushNode(&nodes[pointD]);

        m_curveModel->EmplaceFace(face);

        m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

        return true;
    }
    }

    return false;
}
bool InsertFaceAction::Revert()
{
    if (m_faceIndex != -1)
    {
        Node3Cluster* nodes = m_curveModel->GetNodes();
        const CurveFace face = m_curveModel->GetFace(m_faceIndex);

        switch (face.FaceMode)
        {
        case FaceMode_3Point:
        {
            const std::vector<NodeGroup>::iterator iterAB = nodes[face.Index[FaceIndex_3Point_AB]].Nodes.begin() + face.ClusterIndex[FaceIndex_3Point_AB];
            const std::vector<NodeGroup>::iterator iterAC = nodes[face.Index[FaceIndex_3Point_AC]].Nodes.begin() + face.ClusterIndex[FaceIndex_3Point_AC];
            const std::vector<NodeGroup>::iterator iterBA = nodes[face.Index[FaceIndex_3Point_BA]].Nodes.begin() + face.ClusterIndex[FaceIndex_3Point_BA];
            const std::vector<NodeGroup>::iterator iterBC = nodes[face.Index[FaceIndex_3Point_BC]].Nodes.begin() + face.ClusterIndex[FaceIndex_3Point_BC];
            const std::vector<NodeGroup>::iterator iterCA = nodes[face.Index[FaceIndex_3Point_CA]].Nodes.begin() + face.ClusterIndex[FaceIndex_3Point_CA];
            const std::vector<NodeGroup>::iterator iterCB = nodes[face.Index[FaceIndex_3Point_CB]].Nodes.begin() + face.ClusterIndex[FaceIndex_3Point_CB];

            PopNode(&(nodes[face.Index[FaceIndex_3Point_AB]]), iterAB);
            PopNode(&(nodes[face.Index[FaceIndex_3Point_AC]]), iterAC);
            PopNode(&(nodes[face.Index[FaceIndex_3Point_BA]]), iterBA);
            PopNode(&(nodes[face.Index[FaceIndex_3Point_BC]]), iterBC);
            PopNode(&(nodes[face.Index[FaceIndex_3Point_CA]]), iterCA);
            PopNode(&(nodes[face.Index[FaceIndex_3Point_CB]]), iterCB);

            break;
        }
        case FaceMode_4Point:
        {
            const std::vector<NodeGroup>::iterator iterAB = nodes[face.Index[FaceIndex_4Point_AB]].Nodes.begin() + face.ClusterIndex[FaceIndex_4Point_AB];
            const std::vector<NodeGroup>::iterator iterAC = nodes[face.Index[FaceIndex_4Point_AC]].Nodes.begin() + face.ClusterIndex[FaceIndex_4Point_AC];
            const std::vector<NodeGroup>::iterator iterBA = nodes[face.Index[FaceIndex_4Point_BA]].Nodes.begin() + face.ClusterIndex[FaceIndex_4Point_BA];
            const std::vector<NodeGroup>::iterator iterBD = nodes[face.Index[FaceIndex_4Point_BD]].Nodes.begin() + face.ClusterIndex[FaceIndex_4Point_BD];
            const std::vector<NodeGroup>::iterator iterCA = nodes[face.Index[FaceIndex_4Point_CA]].Nodes.begin() + face.ClusterIndex[FaceIndex_4Point_CA];
            const std::vector<NodeGroup>::iterator iterCD = nodes[face.Index[FaceIndex_4Point_CD]].Nodes.begin() + face.ClusterIndex[FaceIndex_4Point_CD];
            const std::vector<NodeGroup>::iterator iterDB = nodes[face.Index[FaceIndex_4Point_DB]].Nodes.begin() + face.ClusterIndex[FaceIndex_4Point_DB];
            const std::vector<NodeGroup>::iterator iterDC = nodes[face.Index[FaceIndex_4Point_DC]].Nodes.begin() + face.ClusterIndex[FaceIndex_4Point_DC];

            PopNode(&(nodes[face.Index[FaceIndex_4Point_AB]]), iterAB);
            PopNode(&(nodes[face.Index[FaceIndex_4Point_AC]]), iterAC);
            PopNode(&(nodes[face.Index[FaceIndex_4Point_BA]]), iterBA);
            PopNode(&(nodes[face.Index[FaceIndex_4Point_BD]]), iterBD);
            PopNode(&(nodes[face.Index[FaceIndex_4Point_CA]]), iterCA);
            PopNode(&(nodes[face.Index[FaceIndex_4Point_CD]]), iterCD);
            PopNode(&(nodes[face.Index[FaceIndex_4Point_DB]]), iterDB);
            PopNode(&(nodes[face.Index[FaceIndex_4Point_DC]]), iterDC);

            break;
        }
        }

        m_curveModel->DestroyFace(m_faceIndex);

        m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

        m_faceIndex = -1;
    }

    return true;
}