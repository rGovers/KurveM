#include "Physics/CollisionObjects/Softbody.h"

#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "CurveModel.h"
#include "Object.h"
#include "PathModel.h"
#include "Physics/ArmatureBody.h"
#include "Physics/CollisionShapes/CollisionShape.h"
#include "Physics/TransformMotionState.h"
#include "PhysicsEngine.h"
#include "ShaderStorageBuffer.h"
#include "Transform.h"
#include "Workspace.h"

Softbody::Softbody(Object* a_object, Workspace* a_workspace, PhysicsEngine* a_engine) : CollisionObject(a_object, a_engine)
{
    m_workspace = a_workspace;
    m_deltaData = nullptr;

    m_lineMat = nullptr;
    m_faceMat = nullptr;

    m_mass = 1.0f;

    m_dampening = 0.0f;

    m_stiffness = 1.0f;
    m_angularStiffness = 1.0f;
    m_volumeStiffness = 1.0f;

    m_faceStiffness = 1.0f;
    m_faceAngularStiffness = 1.0f;
    m_faceVolumeStiffness = 1.0f;
}
Softbody::~Softbody()
{
    if (m_collisionObject != nullptr)
    {
        ((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->removeSoftBody((btSoftBody*)m_collisionObject);

        delete m_collisionObject;
        m_collisionObject = nullptr;

        m_lineMat = nullptr;
        m_faceMat = nullptr;
    }

    if (m_deltaData != nullptr)
    {
        delete m_deltaData;
        m_deltaData = nullptr;
    }
}

btSoftBody* Softbody::GenerateBody()
{
    btSoftBody* body = nullptr;

    m_faceMat = nullptr;
    m_lineMat = nullptr;

    const Transform* transform = m_transformState->GetAnimationTransform();
    const glm::mat4 transMat = transform->ToMatrix();

    switch (m_object->GetObjectType())
    {
    case ObjectType_CurveModel:
    {
        const CurveModel* model = m_object->GetCurveModel();
        if (model != nullptr)
        {
            const unsigned int nodeCount = model->GetNodeCount();
            const CurveNodeCluster* nodes = model->GetNodes();

            btVector3* nP = new btVector3[nodeCount];
            btScalar* nM = new btScalar[nodeCount];
            for (unsigned int i = 0; i < nodeCount; ++i)
            {
                const BezierCurveNode3& node = nodes[i].Nodes[0].Node;

                const glm::vec4 pos = transMat * glm::vec4(node.GetPosition(), 1.0f);
                nP[i] = btVector3(pos.x, pos.y, pos.z);
                nM[i] = 1.0f;
            }

            body = new btSoftBody(&((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->getWorldInfo(), (int)nodeCount, nP, nM);

            for (int i = 0; i < nodeCount; ++i)
            {
                const std::vector<BoneCluster> bones = nodes[i].Nodes[0].Node.GetBones();

                for (auto iter = bones.begin(); iter != bones.end(); ++iter)
                {
                    Object* obj = m_workspace->GetObject(iter->ID);
                    if (obj != nullptr)
                    {
                        const ArmatureBody* armBody = obj->GetArmatureBody(m_engine);
                        if (armBody != nullptr)
                        {
                            body->appendAnchor(i, armBody->GetRigidbody(), true, iter->Weight);
                        }
                    }
                }
            }

            m_lineMat = body->appendMaterial();
            m_faceMat = body->appendMaterial();

            delete[] nP;
            delete[] nM;

            const unsigned int faceCount = model->GetFaceCount();
            const CurveFace* faces = model->GetFaces();

            for (unsigned int i = 0; i < faceCount; ++i)
            {
                const CurveFace face = faces[i];
                switch (face.FaceMode)
                {
                case FaceMode_3Point:
                {
                    const int indexA = (int)face.Index[FaceIndex_3Point_AB];
                    const int indexB = (int)face.Index[FaceIndex_3Point_BC];
                    const int indexC = (int)face.Index[FaceIndex_3Point_CA];

                    body->appendLink(indexA, indexB, m_lineMat);
                    body->appendLink(indexB, indexC, m_lineMat);
                    body->appendLink(indexC, indexA, m_lineMat);

                    body->appendFace(indexA, indexB, indexC, m_faceMat);

                    break;
                }
                case FaceMode_4Point:
                {
                    const int indexA = (int)face.Index[FaceIndex_4Point_AB];
                    const int indexB = (int)face.Index[FaceIndex_4Point_BD];
                    const int indexC = (int)face.Index[FaceIndex_4Point_DC];
                    const int indexD = (int)face.Index[FaceIndex_4Point_CA];

                    body->appendLink(indexA, indexB, m_lineMat);
                    body->appendLink(indexB, indexC, m_lineMat);
                    body->appendLink(indexC, indexD, m_lineMat);
                    body->appendLink(indexD, indexA, m_lineMat);

                    body->appendFace(indexA, indexB, indexC, m_faceMat);
                    body->appendFace(indexB, indexD, indexC, m_faceMat);

                    break;
                }
                }
            }
        }
    }
    case ObjectType_PathModel:
    {
        const PathModel* model = m_object->GetPathModel();
        if (model != nullptr)
        {
            const unsigned int nodeCount = model->GetPathNodeCount();
            const PathNodeCluster* nodes = model->GetPathNodes();

            btVector3* nP = new btVector3[nodeCount];
            btScalar* nM = new btScalar[nodeCount];
            for (unsigned int i = 0; i < nodeCount; ++i)
            {
                const BezierCurveNode3& node = nodes[i].Nodes[0].Node;
                const glm::vec4 pos = transMat * glm::vec4(node.GetPosition(), 1.0f);
                nP[i] = btVector3(pos.x, pos.y, pos.z);
                nM[i] = 1.0f;
            }

            body = new btSoftBody(&((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->getWorldInfo(), (int)nodeCount, nP, nM);

            m_lineMat = body->appendMaterial();

            delete[] nP;
            delete[] nM;

            const unsigned int lineCount = model->GetPathLineCount();
            const PathLine* lines = model->GetPathLines();

            for (unsigned int i = 0; i < lineCount; ++i)
            {
                const PathLine& line = lines[i];
                body->appendLink((int)line.Index[0], (int)line.Index[1], m_lineMat);
            }

            for (int i = 0; i < nodeCount; ++i)
            {
                const std::vector<BoneCluster> bones = nodes[i].Nodes[0].Node.GetBones();

                for (auto iter = bones.begin(); iter != bones.end(); ++iter)
                {
                    Object* obj = m_workspace->GetObject(iter->ID);
                    if (obj != nullptr)
                    {
                        const ArmatureBody* armBody = obj->GetArmatureBody(m_engine);
                        if (armBody != nullptr)
                        {
                            body->appendAnchor(i, armBody->GetRigidbody(), true, iter->Weight);
                        }
                    }
                }
            }
        }

        break;
    }
    }

    if (m_lineMat != nullptr)
    {
        m_lineMat->m_kAST = m_angularStiffness;
        m_lineMat->m_kLST = m_stiffness;
        m_lineMat->m_kVST = m_volumeStiffness;
    }
    
    if (m_faceMat != nullptr)
    {
        m_faceMat->m_kAST = m_faceAngularStiffness;
        m_faceMat->m_kLST = m_faceStiffness;
        m_faceMat->m_kVST = m_faceVolumeStiffness;
    }
    
    body->m_cfg.kDP = m_dampening;

    body->setTotalMass(m_mass);

    return body;
}

void Softbody::SetActiveState(bool a_state)
{
    m_isActive = a_state;

    m_lineMat = nullptr;
    m_faceMat = nullptr;

    if (m_isActive && m_collisionObject == nullptr)
    {
        btSoftBody* body = GenerateBody();

        m_collisionObject = body;
        ((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->addSoftBody(body);
    }
    else if (m_collisionObject != nullptr)
    {
        ((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->removeSoftBody((btSoftBody*)m_collisionObject);

        delete m_collisionObject;
        m_collisionObject = nullptr;
    }
}

e_CollisionObjectType Softbody::GetCollisionObjectType()
{
    return CollisionObjectType_Softbody;
}

void Softbody::SetMass(float a_value)
{
    m_mass = a_value;
    
    if (m_collisionObject != nullptr)
    {
        ((btSoftBody*)m_collisionObject)->setTotalMass(m_mass);
    }
}

void Softbody::SetDampening(float a_value)
{
    m_dampening = a_value;

    if (m_collisionObject != nullptr)
    {
        ((btSoftBody*)m_collisionObject)->m_cfg.kDP = m_dampening;
    }
}

void Softbody::SetLineStiffness(float a_value)
{
    m_stiffness = a_value;

    if (m_lineMat != nullptr)
    {
        m_lineMat->m_kLST = m_stiffness;
    }
}
void Softbody::SetLineAngularStiffness(float a_value)
{
    m_angularStiffness = a_value;

    if (m_lineMat != nullptr)
    {
        m_lineMat->m_kAST = m_angularStiffness;
    }
}
void Softbody::SetLineVolumeStiffness(float a_value)
{
    m_volumeStiffness = a_value;
    
    if (m_lineMat != nullptr)
    {
        m_lineMat->m_kVST = m_volumeStiffness;
    }
}

void Softbody::SetFaceStiffness(float a_value)
{
    m_faceStiffness = a_value;

    if (m_faceMat != nullptr)
    {
        m_faceMat->m_kLST = m_faceStiffness;
    }
}
void Softbody::SetFaceAngularStiffness(float a_value)
{
    m_faceAngularStiffness = a_value;

    if (m_faceMat != nullptr)
    {
        m_faceMat->m_kAST = m_faceAngularStiffness;
    }
}
void Softbody::SetFaceVolumeStiffness(float a_value)
{
    m_faceVolumeStiffness = a_value;

    if (m_faceMat != nullptr)
    {
        m_faceMat->m_kVST = m_faceVolumeStiffness;
    }
}

void Softbody::Reset()
{
    if (m_collisionObject != nullptr)
    {
        ((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->removeSoftBody((btSoftBody*)m_collisionObject);

        delete m_collisionObject;
        m_collisionObject = nullptr;
    }

    m_lineMat = nullptr;
    m_faceMat = nullptr;

    if (m_isActive)
    {
        btSoftBody* body = GenerateBody();

        m_collisionObject = body;
        ((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->addSoftBody(body);
    }
}

void Softbody::UpdateDeltaStorageBuffer()
{
    if (m_collisionObject != nullptr)
    {   
        const btSoftBody::tNodeArray& arr = ((btSoftBody*)m_collisionObject)->m_nodes;
        const int size = arr.size();

        glm::vec4* verts = new glm::vec4[size];
        switch (m_object->GetObjectType())
        {
        case ObjectType_CurveModel:
        {
            const CurveModel* model = m_object->GetCurveModel();
            if (model != nullptr)
            {
                const CurveNodeCluster* nodes = model->GetNodes();

                for (int i = 0; i < size; ++i)
                {
                    const btVector3 &p = arr[i].m_x;

                    verts[i] = glm::vec4(p.getX(), p.getY(), p.getZ(), 0.0f) - glm::vec4(nodes[i].Nodes[0].Node.GetPosition(), 0.0f);
                }
            }

            break;
        }
        case ObjectType_PathModel:
        {
            const PathModel* model = m_object->GetPathModel();
            if (model != nullptr)
            {
                const PathNodeCluster* nodes = model->GetPathNodes();

                for (int i = 0; i < size; ++i)
                {
                    const btVector3& p = arr[i].m_x;

                    verts[i] = glm::vec4(p.getX(), p.getY(), p.getZ(), 0.0f) - glm::vec4(nodes[i].Nodes[0].Node.GetPosition(), 0.0f);
                }
            }

            break;
        }
        }

        const unsigned int bufferSize = (unsigned int)(sizeof(glm::vec4) * size);

        if (m_deltaData == nullptr)
        {
            m_deltaData = new ShaderStorageBuffer(verts, bufferSize);
        }
        else
        {
            m_deltaData->WriteData(verts, bufferSize);
        }
    }
}
