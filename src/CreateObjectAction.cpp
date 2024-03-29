#include "Actions/CreateObjectAction.h"

#include <string.h>
#include <vector>

#include "CurveModel.h"
#include "EditorControls/Editor.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "LongTasks/TriangulatePathLongTask.h"
#include "Object.h"
#include "PathModel.h"
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
    case CreateObjectType_SpiralPath:
    {
        return "Path Spiral";
    }
    case CreateObjectType_TorusPath:
    {
        return "Path Torus";
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

e_ActionType CreateObjectAction::GetActionType() const
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
    CurveNodeCluster* curveNodePtr;
    unsigned int curveFaceCount = 0;
    CurveFace* curveFacePtr;

    PathNodeCluster* pathNodePtr;
    unsigned int pathLineCount = 0;
    PathLine* pathLinesPtr;
    
    unsigned int shapeNodeCount = 0;
    ShapeNodeCluster* shapeNodePtr;
    unsigned int shapeLineCount = 0;
    ShapeLine* shapeLinesPtr;

    e_ObjectType objectType = ObjectType_Empty;

    switch (m_objectType)
    {
        case CreateObjectType_TriangleCurve:
        {
            PrimitiveGenerator::CreateCurveTriangle(&curveNodePtr, &nodeCount, &curveFacePtr, &curveFaceCount);

            objectType = ObjectType_CurveModel;

            break;
        }
        case CreateObjectType_PlaneCurve:
        {
            PrimitiveGenerator::CreateCurvePlane(&curveNodePtr, &nodeCount, &curveFacePtr, &curveFaceCount);

            objectType = ObjectType_CurveModel;

            break;
        }
        case CreateObjectType_SphereCurve:
        {
            PrimitiveGenerator::CreateCurveSphere(&curveNodePtr, &nodeCount, &curveFacePtr, &curveFaceCount);

            objectType = ObjectType_CurveModel;

            break;
        }
        case CreateObjectType_CubeCurve:
        {
            PrimitiveGenerator::CreateCurveCube(&curveNodePtr, &nodeCount, &curveFacePtr, &curveFaceCount);

            objectType = ObjectType_CurveModel;

            break;
        }
        case CreateObjectType_CylinderPath:
        {
            PrimitiveGenerator::CreatePathCylinder(&pathNodePtr, &nodeCount, &pathLinesPtr, &pathLineCount, &shapeNodePtr, &shapeNodeCount, &shapeLinesPtr, &shapeLineCount);

            objectType = ObjectType_PathModel;

            break;
        }
        case CreateObjectType_ConePath:
        {
            PrimitiveGenerator::CreatePathCone(&pathNodePtr, &nodeCount, &pathLinesPtr, &pathLineCount, &shapeNodePtr, &shapeNodeCount, &shapeLinesPtr, &shapeLineCount);

            objectType = ObjectType_PathModel;

            break;
        }
        case CreateObjectType_SpiralPath:
        {
            PrimitiveGenerator::CreatePathSpiral(&pathNodePtr, &nodeCount, &pathLinesPtr, &pathLineCount, &shapeNodePtr, &shapeNodeCount, &shapeLinesPtr, &shapeLineCount);

            objectType = ObjectType_PathModel;

            break;
        }
        case CreateObjectType_TorusPath:
        {
            PrimitiveGenerator::CreatePathTorus(&pathNodePtr, &nodeCount, &pathLinesPtr, &pathLineCount, &shapeNodePtr, &shapeNodeCount, &shapeLinesPtr, &shapeLineCount);

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
        if (nodeCount != 0 && curveFaceCount != 0)
        {
            CurveModel* model = new CurveModel(m_workspace);

            model->PassModelData(curveNodePtr, nodeCount, curveFacePtr, curveFaceCount);

            m_workspace->PushLongTask(new TriangulateCurveLongTask(model));

            m_object->SetCurveModel(model);
        }

        break;
    }
    case ObjectType_PathModel:
    {
        if (nodeCount != 0 && pathLineCount != 0 && shapeNodeCount != 0 && shapeLineCount != 0)
        {
            PathModel* model = new PathModel(m_workspace);

            model->PassModelData(pathNodePtr, nodeCount, pathLinesPtr, pathLineCount, shapeNodePtr, shapeNodeCount, shapeLinesPtr, shapeLineCount);

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