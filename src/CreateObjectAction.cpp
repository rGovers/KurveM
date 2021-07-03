#include "Actions/CreateObjectAction.h"

#include <vector>

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Object.h"
#include "PrimitiveGenerator.h"
#include "Workspace.h"

CreateObjectAction::CreateObjectAction(Workspace* a_workspace, Object* a_parent, e_CreateObjectType a_type)
{
    m_workspace = a_workspace;

    m_parentObject = a_parent;
    m_object = nullptr;

    m_objectType = a_type;
}
CreateObjectAction::~CreateObjectAction()
{

}

e_ActionType CreateObjectAction::GetActionType()
{
    return ActionType_CreateObject;
}

bool CreateObjectAction::Redo()
{
    return Execute();
}
bool CreateObjectAction::Execute()
{
    if (m_object == nullptr)
    {
        m_object = new Object("New Object");

        if (m_parentObject != nullptr)
        {
            m_object->SetParent(m_parentObject);
        }
        else
        {
            m_workspace->AddObject(m_object);
        }

        m_workspace->ClearSelectedObjects();
        m_workspace->AddSelectedObject(m_object);
    }

    unsigned int nodeCount = 0;
    Node3Cluster* nodePtr;
    unsigned int faceCount = 0;
    CurveFace* facePtr;

    switch (m_objectType)
    {
        case CreateObjectType_SphereCurve:
        {
            PrimitiveGenerator::CreateCurveSphere(&nodePtr, &nodeCount, &facePtr, &faceCount);

            break;
        }
        case CreateObjectType_CubeCurve:
        {
            PrimitiveGenerator::CreateCurveCube(&nodePtr, &nodeCount, &facePtr, &faceCount);

            break;
        }
    }

    if (nodeCount != 0 && faceCount != 0)
    {
        CurveModel* model = new CurveModel();

        model->SetModelData(nodePtr, nodeCount, facePtr, faceCount);

        m_workspace->PushLongTask(new TriangulateCurveLongTask(model));

        m_object->SetCurveModel(model);
    }

    return true;
}
bool CreateObjectAction::Revert()
{
    if (m_object != nullptr)
    {
        if (m_parentObject != nullptr)
        {
            m_object->SetParent(nullptr);
        }
        else
        {
            m_workspace->RemoveObject(m_object);
        }
        
        delete m_object;
        m_object = nullptr;
    }

    return true;
}