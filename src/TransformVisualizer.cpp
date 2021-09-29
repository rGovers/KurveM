#include "TransformVisualizer.h"

#include "LocalModel.h"
#include "PrimitiveGenerator.h"

TransformVisualizer* TransformVisualizer::Instance = nullptr;

TransformVisualizer::TransformVisualizer()
{
    Vertex* vertices;
    unsigned int vertexCount;
    unsigned int* indices;
    unsigned int indexCount;
    
    PrimitiveGenerator::CreateCylinder(&vertices, &vertexCount, &indices, &indexCount, 0.025f, 6, 1.0f, glm::vec3(0, 1, 0));

    m_arm = new LocalModel(vertices, indices, vertexCount, indexCount);

    delete[] vertices;
    delete[] indices;

    PrimitiveGenerator::CreateCone(&vertices, &vertexCount, &indices, &indexCount, 0.1f, 10, 0.2f, glm::vec3(0, 1, 0));

    m_translationHandle = new LocalModel(vertices, indices, vertexCount, indexCount);

    delete[] vertices;
    delete[] indices;

    PrimitiveGenerator::CreateIcoSphere(&vertices, &vertexCount, &indices, &indexCount, 0.1f, 1);

    m_scaleHandle = new LocalModel(vertices, indices, vertexCount, indexCount);

    delete[] vertices;
    delete[] indices;

    PrimitiveGenerator::CreateTorus(&vertices, &vertexCount, &indices, &indexCount, 1, 30, 0.05f, 6, glm::vec3(0, 1, 0));

    m_rotationHandle = new LocalModel(vertices, indices, vertexCount, indexCount);

    delete[] vertices;
    delete[] indices;
}
TransformVisualizer::~TransformVisualizer()
{
    delete m_arm;

    delete m_translationHandle;
    delete m_scaleHandle;
    delete m_rotationHandle;
}

void TransformVisualizer::Init()
{
    if (Instance == nullptr)
    {
        Instance = new TransformVisualizer();
    }
}
void TransformVisualizer::Destroy()
{
    if (Instance != nullptr)
    {
        delete Instance;
        Instance = nullptr;
    }
}