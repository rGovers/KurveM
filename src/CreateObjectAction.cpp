#include "Actions/CreateObjectAction.h"

#include <string.h>
#include <vector>

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "LongTasks/TriangulatePathLongTask.h"
#include "Object.h"
#include "PrimitiveGenerator.h"
#include "Workspace.h"

CreateObjectAction::CreateObjectAction(Workspace* a_workspace, Object* a_parent, e_CreateObjectType a_type)
{
    m_workspace = a_workspace;

    m_parentObject = a_parent;
    m_object = nullptr;

    m_texPath = nullptr;

    m_objectType = a_type;
}
CreateObjectAction::CreateObjectAction(Workspace* a_workspace, Object* a_parent, const char* a_texPath)
{
    m_workspace = a_workspace;

    m_parentObject = a_parent;
    m_object = nullptr;

    m_texPath = a_texPath;

    m_objectType = CreateObjectType_ReferenceImage;
}
CreateObjectAction::~CreateObjectAction()
{

}

const char* CreateObjectAction::GetName() const
{
    switch (m_objectType)
    {
    case CreateObjectType_TriangleCurve:
    {
        return "Curve Triangle";
    }
    case CreateObjectType_PlaneCurve:
    {
        return "Curve Plane";
    }
    case CreateObjectType_SphereCurve:
    {
        return "Curve Sphere";
    }
    case CreateObjectType_CubeCurve:
    {
        return "Curve Cube";
    }
    case CreateObjectType_CylinderPath:
    {
        return "Path Cylinder";
    }
    case CreateObjectType_ConePath:
    {
        return "Path Cone";
    }
    case CreateObjectType_ReferenceImage:
    {
        const char* strPtr = m_texPath;

        while (true)
        {
            const char* nextStr = strchr(strPtr, '/');

            if (nextStr != nullptr)
            {
                const int size = strlen(nextStr);

                if (size > 1)
                {
                    strPtr = nextStr + 1;
                }
                else 
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        return strPtr;
    }
    case CreateObjectType_Armature:
    {
        return "Armature";
    }
    }

    return "New Object";
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
    unsigned int nodeCount = 0;
    Node3Cluster* nodePtr;
    unsigned int faceCount = 0;
    CurveFace* facePtr;

    PathNode* pathNodePtr;
    unsigned int pathIndexCount = 0;
    unsigned int* pathIndicesPtr;
    
    unsigned int shapeNodeCount = 0;
    BezierCurveNode2* shapeNodePtr;
    unsigned int shapeIndexCount = 0;
    unsigned int* shapeIndicesPtr;

    e_ObjectType objectType = ObjectType_Empty;

    switch (m_objectType)
    {
        case CreateObjectType_TriangleCurve:
        {
            PrimitiveGenerator::CreateCurveTriangle(&nodePtr, &nodeCount, &facePtr, &faceCount);

            objectType = ObjectType_CurveModel;

            break;
        }
        case CreateObjectType_PlaneCurve:
        {
            PrimitiveGenerator::CreateCurvePlane(&nodePtr, &nodeCount, &facePtr, &faceCount);

            objectType = ObjectType_CurveModel;

            break;
        }
        case CreateObjectType_SphereCurve:
        {
            PrimitiveGenerator::CreateCurveSphere(&nodePtr, &nodeCount, &facePtr, &faceCount);

            objectType = ObjectType_CurveModel;

            break;
        }
        case CreateObjectType_CubeCurve:
        {
            PrimitiveGenerator::CreateCurveCube(&nodePtr, &nodeCount, &facePtr, &faceCount);

            objectType = ObjectType_CurveModel;

            break;
        }
        case CreateObjectType_CylinderPath:
        {
            PrimitiveGenerator::CreatePathCylinder(&pathNodePtr, &nodeCount, &pathIndicesPtr, &pathIndexCount, &shapeNodePtr, &shapeNodeCount, &shapeIndicesPtr, &shapeIndexCount);

            objectType = ObjectType_PathModel;

            break;
        }
        case CreateObjectType_ConePath:
        {
            PrimitiveGenerator::CreatePathCone(&pathNodePtr, &nodeCount, &pathIndicesPtr, &pathIndexCount, &shapeNodePtr, &shapeNodeCount, &shapeIndicesPtr, &shapeIndexCount);

            objectType = ObjectType_PathModel;

            break;
        }
        case CreateObjectType_ReferenceImage:
        {
            objectType = ObjectType_ReferenceImage;

            break;
        }
        case CreateObjectType_Armature:
        {
            objectType = ObjectType_Armature;

            break;
        }
    }

    if (m_object == nullptr)
    {
        m_object = new Object(GetName(), objectType);

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

    switch (objectType)
    {
    case ObjectType_CurveModel:
    {
        if (nodeCount != 0 && faceCount != 0)
        {
            CurveModel* model = new CurveModel(m_workspace);

            model->PassModelData(nodePtr, nodeCount, facePtr, faceCount);

            m_workspace->PushLongTask(new TriangulateCurveLongTask(model));

            m_object->SetCurveModel(model);
        }

        break;
    }
    case ObjectType_PathModel:
    {
        if (nodeCount != 0 && pathIndexCount != 0 && shapeNodeCount != 0 && shapeIndexCount != 0)
        {
            PathModel* model = new PathModel(m_workspace);

            model->PassModelData(pathNodePtr, nodeCount, pathIndicesPtr, pathIndexCount, shapeNodePtr, shapeNodeCount, shapeIndicesPtr, shapeIndexCount);

            m_workspace->PushLongTask(new TriangulatePathLongTask(model));

            m_object->SetPathModel(model);
        }

        break;
    }
    case ObjectType_ReferenceImage:
    {
        if (!m_object->SetReferenceImage(m_texPath))
        {
            printf("Error Creating Reference Image \n");

            return false;
        } 

        break;
    }
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

        m_workspace->ClearSelectedObjects();
        
        delete m_object;
        m_object = nullptr;
    }

    return true;
}