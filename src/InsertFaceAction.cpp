#include "Actions/InsertFaceAction.h"

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

void InsertFaceAction::Wind3Points(const Node3Cluster* a_nodes, unsigned int* a_a, unsigned int* a_b, unsigned int* a_c) const
{
    const glm::vec3 posA = a_nodes[*a_a].Nodes[0].Node.GetPosition();
    const glm::vec3 posB = a_nodes[*a_b].Nodes[0].Node.GetPosition();
    const glm::vec3 posC = a_nodes[*a_c].Nodes[0].Node.GetPosition();

    const glm::vec3 mid = (posA + posB + posC) * 0.33f;

    const glm::vec3 normA = glm::normalize(posA - mid);
    const glm::vec3 normB = glm::normalize(posB - mid);
    const glm::vec3 normC = glm::normalize(posC - mid);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(up, normA)) >= 0.95f)
    {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    const glm::vec3 forward = glm::cross(normA, up);
    up = glm::cross(normA, forward);

    const glm::mat3 invMat = glm::inverse(glm::mat3(normA, up, forward));

    const glm::vec3 pNormA = invMat * normA;
    const glm::vec3 pNormB = invMat * normB;
    const glm::vec3 pNormC = invMat * normC;

    float angleA = atan2(pNormA.x, pNormA.y);
    float angleB = atan2(pNormB.x, pNormB.y);
    float angleC = atan2(pNormC.x, pNormC.y);

    if (angleB > angleC)
    {
        const unsigned int temp = *a_b;
        *a_b = *a_c;
        *a_c = temp;

        const float tempAngle = angleB;
        angleB = angleC;
        angleC = tempAngle;
    }

    if (angleA > angleB)
    {
        if (angleA > angleC)
        {
            const unsigned int temp = *a_c;
            *a_c = *a_a;
            *a_a = temp;
        }

        const unsigned int temp = *a_b;
        *a_b = *a_a;
        *a_a = temp;
    }
}

void InsertFaceAction::Wind4Points(const Node3Cluster* a_nodes, unsigned int* a_a, unsigned int* a_b, unsigned int* a_c, unsigned int* a_d) const
{
    const glm::vec3 posA = a_nodes[*a_a].Nodes[0].Node.GetPosition();
    const glm::vec3 posB = a_nodes[*a_b].Nodes[0].Node.GetPosition();
    const glm::vec3 posC = a_nodes[*a_c].Nodes[0].Node.GetPosition();
    const glm::vec3 posD = a_nodes[*a_d].Nodes[0].Node.GetPosition();

    const glm::vec3 mid = (posA + posB + posC + posD) * 0.25f;

    const glm::vec3 normA = glm::normalize(posA - mid);
    const glm::vec3 normB = glm::normalize(posB - mid);
    const glm::vec3 normC = glm::normalize(posC - mid);
    const glm::vec3 normD = glm::normalize(posD - mid);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(up, normA)) >= 0.95f)
    {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    const glm::vec3 forward = glm::cross(normA, up);
    up = glm::cross(normA, forward);

    const glm::mat3 invMat = glm::inverse(glm::mat3(normA, up, forward));

    const glm::vec3 pNormA = invMat * normA;
    const glm::vec3 pNormB = invMat * normB;
    const glm::vec3 pNormC = invMat * normC;
    const glm::vec3 pNormD = invMat * normD;

    float angleA = atan2(pNormA.x, pNormA.y);
    float angleB = atan2(pNormB.x, pNormB.y);
    float angleC = atan2(pNormC.x, pNormC.y);
    float angleD = atan2(pNormD.x, pNormD.y);

    if (angleC > angleD)
    {
        const unsigned int tmp = *a_c;
        *a_c = *a_d;
        *a_d = tmp;

        const float tmpAngle = angleC;
        angleC = angleD;
        angleD = tmpAngle;
    }

    if (angleB > angleC)
    {
        if (angleB > angleD)
        {
            const unsigned int tmp = *a_b;
            *a_b = *a_d;
            *a_d = tmp;

            const float tmpAngle = angleB;
            angleB = angleD;
            angleD = tmpAngle;
        }

        const unsigned int tmp = *a_b;
        *a_b = *a_c;
        *a_c = tmp;

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
                const unsigned int tmp = *a_a;
                *a_a = *a_d;
                *a_d = tmp;
            }

            const unsigned int tmp = *a_a;
            *a_a = *a_c;
            *a_c = tmp;
        }

        const unsigned int tmp = *a_a;
        *a_a = *a_b;
        *a_b = tmp;
    }
}

bool InsertFaceAction::GetIndex(const CurveFace& a_face, e_FaceIndex a_faceIndexA, e_FaceIndex a_faceIndexB, unsigned int a_indexA, unsigned int a_indexB, unsigned int* a_out) const
{
    const unsigned int indA = a_face.Index[a_faceIndexA];
    const unsigned int indB = a_face.Index[a_faceIndexB];

    if (indA == a_indexA && indB == a_indexB)
    {
        *a_out = a_face.ClusterIndex[a_faceIndexA];

        return true;
    }
    else if (indB == a_indexA && indA == a_indexB)
    {
        *a_out = a_face.ClusterIndex[a_faceIndexB];

        return true;
    }

    return false;
}
unsigned int InsertFaceAction::PushNode(Node3Cluster* a_cluster, unsigned int a_startIndex, unsigned int a_endIndex, const CurveFace* a_faces, unsigned int a_faceCount) const
{
    constexpr float infinity = std::numeric_limits<float>::infinity();

    for (unsigned int i = 0; i < a_faceCount; ++i)
    {
        const CurveFace face = a_faces[i];
        switch (face.FaceMode)
        {
        case FaceMode_3Point:
        {
            unsigned int index;
            if (GetIndex(face, FaceIndex_3Point_AB, FaceIndex_3Point_BA, a_startIndex, a_endIndex, &index))
            {
                return index;
            }
            else if (GetIndex(face, FaceIndex_3Point_AC, FaceIndex_3Point_CA, a_startIndex, a_endIndex, &index))
            {
                return index;
            }
            else if (GetIndex(face, FaceIndex_3Point_BC, FaceIndex_3Point_CB, a_startIndex, a_endIndex, &index))
            {
                return index;
            }

            break;
        }
        case FaceMode_4Point:
        {
            unsigned int index;
            if (GetIndex(face, FaceIndex_4Point_AB, FaceIndex_4Point_BA, a_startIndex, a_endIndex, &index))
            {
                return index;
            }
            else if (GetIndex(face, FaceIndex_4Point_AC, FaceIndex_4Point_CA, a_startIndex, a_endIndex, &index))
            {
                return index;
            }
            else if (GetIndex(face, FaceIndex_4Point_BD, FaceIndex_4Point_DB, a_startIndex, a_endIndex, &index))
            {
                return index;
            }
            else if (GetIndex(face, FaceIndex_4Point_CD, FaceIndex_4Point_DC, a_startIndex, a_endIndex, &index))
            {
                return index;
            }

            break;
        }
        }
    }

    const unsigned int size = (unsigned int)a_cluster->Nodes.size();

    for (unsigned int i = 0; i < size; ++i)
    {
        NodeGroup& node = a_cluster->Nodes[i];
        if (node.Node.GetHandlePosition().x == infinity)
        {
            node.Node.SetHandlePosition(a_cluster->Nodes[i].Node.GetPosition());

            return i;
        }
    }

    const glm::vec3 pos = a_cluster->Nodes[0].Node.GetPosition();

    BezierCurveNode3 node;
    node.SetPosition(pos);
    node.SetHandlePosition(pos);

    a_cluster->Nodes.emplace_back(node);

    return size;
}

void InsertFaceAction::PopNode(Node3Cluster* a_cluster, unsigned int a_index) const
{
    constexpr float infinity = std::numeric_limits<float>::infinity();

    NodeGroup& nGroup = a_cluster->Nodes[a_index];

    if (--nGroup.FaceCount <= 0)
    {
        nGroup.Node.SetHandlePosition(glm::vec3(infinity));
        nGroup.FaceCount = 0;
    }
}

InsertFaceAction::InsertFaceAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel)
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
    const CurveFace* faces = m_curveModel->GetFaces();
    const unsigned int faceCount = m_curveModel->GetFaceCount();

    switch (m_nodeCount)
    {
    case 3:
    {
        unsigned int pointA = m_nodeIndices[0];
        unsigned int pointB = m_nodeIndices[1];
        unsigned int pointC = m_nodeIndices[2];
        
        Wind3Points(nodes, &pointA, &pointB, &pointC);

        m_faceIndex = m_curveModel->GetFaceCount();

        CurveFace face;

        face.FaceMode = FaceMode_3Point;

        face.Index[FaceIndex_3Point_AB] = pointA;
        face.Index[FaceIndex_3Point_AC] = pointA;
        face.Index[FaceIndex_3Point_BA] = pointB;
        face.Index[FaceIndex_3Point_BC] = pointB;
        face.Index[FaceIndex_3Point_CA] = pointC;
        face.Index[FaceIndex_3Point_CB] = pointC;

        face.ClusterIndex[FaceIndex_3Point_AB] = PushNode(&nodes[pointA], pointA, pointB, faces, faceCount);
        face.ClusterIndex[FaceIndex_3Point_AC] = PushNode(&nodes[pointA], pointA, pointC, faces, faceCount);
        face.ClusterIndex[FaceIndex_3Point_BA] = PushNode(&nodes[pointB], pointB, pointA, faces, faceCount);
        face.ClusterIndex[FaceIndex_3Point_BC] = PushNode(&nodes[pointB], pointB, pointC, faces, faceCount);
        face.ClusterIndex[FaceIndex_3Point_CA] = PushNode(&nodes[pointC], pointC, pointA, faces, faceCount);
        face.ClusterIndex[FaceIndex_3Point_CB] = PushNode(&nodes[pointC], pointC, pointB, faces, faceCount);

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

        Wind4Points(nodes, &pointA, &pointB, &pointC, &pointD);

        m_faceIndex = m_curveModel->GetFaceCount();

        CurveFace face;

        face.FaceMode = FaceMode_4Point;

        face.Index[FaceIndex_4Point_AB] = pointA;
        face.Index[FaceIndex_4Point_AC] = pointA;
        face.Index[FaceIndex_4Point_BA] = pointB;
        face.Index[FaceIndex_4Point_BD] = pointB;
        face.Index[FaceIndex_4Point_CA] = pointD;
        face.Index[FaceIndex_4Point_CD] = pointD;
        face.Index[FaceIndex_4Point_DB] = pointC;
        face.Index[FaceIndex_4Point_DC] = pointC;

        face.ClusterIndex[FaceIndex_4Point_AB] = PushNode(&nodes[pointA], pointA, pointB, faces, faceCount);
        face.ClusterIndex[FaceIndex_4Point_AC] = PushNode(&nodes[pointA], pointA, pointC, faces, faceCount);
        face.ClusterIndex[FaceIndex_4Point_BA] = PushNode(&nodes[pointB], pointB, pointA, faces, faceCount);
        face.ClusterIndex[FaceIndex_4Point_BD] = PushNode(&nodes[pointB], pointB, pointD, faces, faceCount);
        face.ClusterIndex[FaceIndex_4Point_CA] = PushNode(&nodes[pointD], pointD, pointA, faces, faceCount);
        face.ClusterIndex[FaceIndex_4Point_CD] = PushNode(&nodes[pointD], pointD, pointC, faces, faceCount);
        face.ClusterIndex[FaceIndex_4Point_DB] = PushNode(&nodes[pointC], pointC, pointB, faces, faceCount);
        face.ClusterIndex[FaceIndex_4Point_DC] = PushNode(&nodes[pointC], pointC, pointD, faces, faceCount);

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
            for (int i = 0; i < 6; ++i)
            {
                PopNode(&nodes[face.Index[i]], face.ClusterIndex[i]);
            }

            break;
        }
        case FaceMode_4Point:
        {
            for (int i = 0; i < 8; ++i)
            {
                PopNode(&nodes[face.Index[i]], face.ClusterIndex[i]);
            }

            break;
        }
        }

        m_curveModel->DestroyFace(m_faceIndex);

        m_faceIndex = -1;
        
        m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));
    }

    return true;
}