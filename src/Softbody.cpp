#include "Physics/CollisionObjects/Softbody.h"

#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "CurveModel.h"
#include "Object.h"
#include "PathModel.h"
#include "Physics/CollisionShapes/CollisionShape.h"
#include "Physics/TransformMotionState.h"
#include "PhysicsEngine.h"
#include "ShaderStorageBuffer.h"

Softbody::Softbody(Object* a_object, PhysicsEngine* a_engine) : CollisionObject()
{
    m_engine = a_engine;

    m_object = a_object;

    m_transformState = new TransformMotionState(m_object);
    m_deltaData = nullptr;

    m_mass = 1.0f;
}
Softbody::~Softbody()
{
    if (m_collisionObject != nullptr)
    {
        ((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->removeSoftBody((btSoftBody*)m_collisionObject);

        delete m_collisionObject;
        m_collisionObject = nullptr;
    }

    if (m_transformState != nullptr)
    {
        delete m_transformState;
        m_transformState = nullptr;
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
                const glm::vec3 pos = nodes[i].Nodes[0].Node.GetPosition();
                nP[i] = btVector3(pos.x, pos.y, pos.z);
                nM[i] = 1.0f;
            }

            body = new btSoftBody(&((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->getWorldInfo(), (int)nodeCount, nP, nM);

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

                    body->appendLink(indexA, indexB);
                    body->appendLink(indexB, indexC);
                    body->appendLink(indexC, indexA);

                    body->appendFace(indexA, indexB, indexC);

                    break;
                }
                case FaceMode_4Point:
                {
                    const int indexA = (int)face.Index[FaceIndex_4Point_AB];
                    const int indexB = (int)face.Index[FaceIndex_4Point_BD];
                    const int indexC = (int)face.Index[FaceIndex_4Point_DC];
                    const int indexD = (int)face.Index[FaceIndex_4Point_CA];

                    body->appendLink(indexA, indexB);
                    body->appendLink(indexB, indexC);
                    body->appendLink(indexC, indexD);
                    body->appendLink(indexD, indexA);

                    body->appendFace(indexA, indexB, indexC);
                    body->appendFace(indexB, indexD, indexC);

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
                const glm::vec3 pos = nodes[i].Nodes[0].Node.GetPosition();
                nP[i] = btVector3(pos.x, pos.y, pos.z);
                nM[i] = 1.0f;
            }

            body = new btSoftBody(&((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->getWorldInfo(), (int)nodeCount, nP, nM);

            delete[] nP;
            delete[] nM;

            const unsigned int lineCount = model->GetPathLineCount();
            const PathLine* lines = model->GetPathLines();

            for (unsigned int i = 0; i < lineCount; ++i)
            {
                const PathLine& line = lines[i];
                body->appendLink((int)line.Index[0], (int)line.Index[1]);
            }
        }

        break;
    }
    }

    body->setTotalMass(m_mass);

    return body;
}

void Softbody::SetActiveState(bool a_state)
{
    m_isActive = a_state;

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

void Softbody::Reset()
{
    if (m_collisionObject != nullptr)
    {
        ((btSoftRigidDynamicsWorld*)m_engine->GetDynamicsWorld())->removeSoftBody((btSoftBody*)m_collisionObject);

        delete m_collisionObject;
        m_collisionObject = nullptr;
    }

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
