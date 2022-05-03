#include "Physics/CollisionShapes/MeshCollisionShape.h"

#include <BulletCollision/Gimpact/btGImpactShape.h>

#include "CurveModel.h"
#include "LocalModel.h"
#include "Object.h"
#include "PathModel.h"
#include "Physics/StridingMeshInterface.h"

MeshCollisionShape::MeshCollisionShape(Object* a_object)
{
    m_object = a_object;

    m_steps = 2;
    m_secSteps = 2;

    m_model = nullptr;

    switch (m_object->GetObjectType())
    {
    case ObjectType_CurveModel:
    {
        const CurveModel* model = m_object->GetCurveModel();
        if (model != nullptr)
        {
            Vertex* vertices; 
            unsigned int vertexCount;
            unsigned int* indices;
            unsigned int indexCount;

            model->GetModelData(false, m_steps, &indices, &indexCount, &vertices, &vertexCount);

            m_model = new LocalModel(vertices, indices, vertexCount, indexCount);
        }

        break;
    }
    case ObjectType_PathModel:
    {
        const PathModel* model = m_object->GetPathModel();
        if (model != nullptr)
        {
            Vertex* vertices; 
            unsigned int vertexCount;
            unsigned int* indices;
            unsigned int indexCount;

            model->GetModelData(m_secSteps, m_steps, &indices, &indexCount, &vertices, &vertexCount);

            m_model = new LocalModel(vertices, indices, vertexCount, indexCount);
        }

        break;
    }
    }

    m_interface = new StridingMeshInterface(m_model);

    btGImpactMeshShape* shape = new btGImpactMeshShape(m_interface);
    shape->updateBound();

    m_shape = shape;
}
MeshCollisionShape::~MeshCollisionShape()
{
    delete m_shape;

    delete m_interface;

    if (m_model != nullptr)
    {
        delete m_model;
        m_model = nullptr;
    }
}

e_CollisionShapeType MeshCollisionShape::GetShapeType()
{
    return CollisionShapeType_Mesh;
}

void MeshCollisionShape::UpdateMesh()
{
    if (m_model != nullptr)
    {
        delete m_model;
        m_model = nullptr;
    }

    switch (m_object->GetObjectType())
    {
    case ObjectType_CurveModel:
    {
        const CurveModel* model = m_object->GetCurveModel();
        if (model != nullptr)
        {
            Vertex* vertices; 
            unsigned int vertexCount;
            unsigned int* indices;
            unsigned int indexCount;

            model->GetModelData(false, m_steps, &indices, &indexCount, &vertices, &vertexCount);

            m_model = new LocalModel(vertices, indices, vertexCount, indexCount);
        }

        break;
    }
    case ObjectType_PathModel:
    {
        const PathModel* model = m_object->GetPathModel();
        if (model != nullptr)
        {
            Vertex* vertices; 
            unsigned int vertexCount;
            unsigned int* indices;
            unsigned int indexCount;

            model->GetModelData(m_secSteps, m_steps, &indices, &indexCount, &vertices, &vertexCount);

            m_model = new LocalModel(vertices, indices, vertexCount, indexCount);
        }

        break;
    }
    }

    m_interface->SetMesh(m_model);

    ((btGImpactMeshShape*)m_shape)->postUpdate();
}
void MeshCollisionShape::Tick()
{
    const glm::mat4* matrices = nullptr;
    unsigned int matrixCount;

    switch (m_object->GetObjectType())
    {
    case ObjectType_CurveModel:
    {
        const CurveModel* model = m_object->GetCurveModel();
        if (model != nullptr)
        {
            const Object* arm = model->GetArmature();
            if (arm != nullptr)
            {
                matrixCount = arm->GetArmatureMatrixCount();
                matrices = arm->GetArmatureMatrices();
            }
        }

        break;
    }
    case ObjectType_PathModel:
    {
        const PathModel* model = m_object->GetPathModel();
        if (model != nullptr)
        {
            const Object* arm = model->GetArmature();
            if (arm != nullptr)
            {
                matrixCount = arm->GetArmatureMatrixCount();
                matrices = arm->GetArmatureMatrices();
            }
        }

        break;
    }
    }

    if (matrices != nullptr)
    {
        m_interface->Tick(matrices, matrixCount);
        ((btGImpactMeshShape*)m_shape)->postUpdate();
    }
}