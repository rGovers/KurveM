#include "IO/PathModelSerializer.h"

#include "IO/MeshExporter.h"
#include "IO/ObjectSerializer.h"
#include "IO/XMLIO.h"
#include "Model.h"
#include "PathModel.h"

void PathModelSerializer::WriteOBJ(std::ofstream* a_file, const PathModel* a_model, int a_pathSteps, int a_shapeSteps)
{
    unsigned int* indices;
    Vertex* vertices;
    unsigned int indexCount;
    unsigned int vertexCount;

    a_model->GetModelData(a_shapeSteps, a_pathSteps, &indices, &indexCount, &vertices, &vertexCount);

    MeshExporter::ExportOBJMesh(a_file, vertices, vertexCount, indices, indexCount);

    delete[] vertices;
    delete[] indices;
}
void PathModelSerializer::WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, tinyxml2::XMLElement* a_parentController, const PathModel* a_model, const char* a_parentID, const char* a_name, int a_pathSteps, int a_shapeSteps, char** a_outRoot)
{
    tinyxml2::XMLElement* meshElement = a_doc->NewElement("mesh");
    a_parent->InsertEndChild(meshElement);

    Vertex* vertices;
    unsigned int* indices;
    unsigned int indexCount;
    unsigned int vertexCount;

    a_model->GetModelData(a_shapeSteps, a_pathSteps, &indices, &indexCount, &vertices, &vertexCount);

    glm::vec4* posVerts;
    glm::vec3* normVerts;
    glm::vec2* uvVerts;
    glm::vec4* bones;
    glm::vec4* weights;

    unsigned int posCount;
    unsigned int normCount;
    unsigned int uvCount;

    std::unordered_map<unsigned int, unsigned int> posMap;
    std::unordered_map<unsigned int, unsigned int> normMap;
    std::unordered_map<unsigned int, unsigned int> uvMap;

    MeshExporter::SplitVertices(vertices, vertexCount, &posVerts, &posCount, &posMap, &normVerts, &normCount, &normMap, &uvVerts, &uvCount, &uvMap, &bones, &weights);

    delete[] vertices;

    MeshExporter::ExportColladaMesh(a_doc, meshElement, posVerts, posCount, normVerts, normCount, uvVerts, uvCount, bones, weights, posMap, normMap, uvMap, indices, indexCount, a_name);

    delete[] posVerts;
    delete[] normVerts;
    delete[] uvVerts;

    delete[] indices;

    if (a_parentController != nullptr)
    {
        const std::list<Object*> armNodes = a_model->GetArmatureNodes();

        MeshExporter::ExportColladaBones(a_doc, a_parentController, bones, weights, posCount, a_parentID, a_name, armNodes, a_outRoot);
    }

    delete[] bones;
    delete[] weights;
}

void PathModelSerializer::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const PathModel* a_model)
{
    tinyxml2::XMLElement* rootElement = a_doc->NewElement("PathModel");
    a_parent->InsertEndChild(rootElement);

    rootElement->SetAttribute("Armature", std::to_string(a_model->GetArmatureID()).c_str());

    tinyxml2::XMLElement* shapeElement = a_doc->NewElement("Shape");
    rootElement->InsertEndChild(shapeElement);
    shapeElement->SetAttribute("Steps", a_model->GetShapeSteps());

    tinyxml2::XMLElement* shapeLinesElement = a_doc->NewElement("Lines");
    shapeElement->InsertEndChild(shapeLinesElement);

    const unsigned int shapeLineCount = a_model->GetShapeLineCount();
    const ShapeLine* shapeLines = a_model->GetShapeLines();
    for (unsigned int i = 0; i < shapeLineCount; ++i)
    {
        const ShapeLine& line = shapeLines[i];

        tinyxml2::XMLElement* lineElement = a_doc->NewElement("Line");
        shapeLinesElement->InsertEndChild(lineElement);

        for (int j = 0; j < 2; ++j)
        {
            tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
            lineElement->InsertEndChild(indexElement);
            indexElement->SetText(line.Index[j]);

            tinyxml2::XMLElement* clusterIndexElement = a_doc->NewElement("ClusterIndex");
            lineElement->InsertEndChild(clusterIndexElement);
            clusterIndexElement->SetText((unsigned int)line.ClusterIndex[j]);
        }
    }

    tinyxml2::XMLElement* shapeNodesElement = a_doc->NewElement("Nodes");
    shapeElement->InsertEndChild(shapeNodesElement);

    const unsigned int shapeNodeCount = a_model->GetShapeNodeCount();
    const ShapeNodeCluster* shapeNodes = a_model->GetShapeNodes();
    for (unsigned int i = 0; i < shapeNodeCount; ++i)
    {
        const ShapeNodeCluster& node = shapeNodes[i];

        const int size = node.Nodes.size();
        if (size > 0)
        {
            tinyxml2::XMLElement* shapeNodeElement = a_doc->NewElement("Node");
            XMLIO::WriteVec2(a_doc, shapeNodeElement, "Position", node.Nodes[0].GetPosition());
            shapeNodesElement->InsertEndChild(shapeNodeElement);

            for (int j = 0; j < size; ++j)
            {
                const BezierCurveNode2& sNode = node.Nodes[j];

                tinyxml2::XMLElement* clusterNodeElement = a_doc->NewElement("ClusterNode");
                shapeNodeElement->InsertEndChild(clusterNodeElement);

                XMLIO::WriteVec2(a_doc, clusterNodeElement, "HandlePosition", sNode.GetHandlePosition());
            }

        }
    }

    tinyxml2::XMLElement* pathElement = a_doc->NewElement("Path");
    rootElement->InsertEndChild(pathElement);
    pathElement->SetAttribute("Steps", a_model->GetPathSteps());

    tinyxml2::XMLElement* pathLinesElement = a_doc->NewElement("Lines");
    pathElement->InsertEndChild(pathLinesElement);

    const unsigned int pathLineCount = a_model->GetPathLineCount();
    const PathLine* pathLines = a_model->GetPathLines();
    for (unsigned int i = 0; i < pathLineCount; ++i)
    {
        const PathLine& line = pathLines[i];

        tinyxml2::XMLElement* lineElement = a_doc->NewElement("Line");
        pathLinesElement->InsertEndChild(lineElement);

        for (int j = 0; j < 2; ++j)
        {
            tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
            lineElement->InsertEndChild(indexElement);
            indexElement->SetText(line.Index[j]);

            tinyxml2::XMLElement* clusterIndexElement = a_doc->NewElement("ClusterIndex");
            lineElement->InsertEndChild(clusterIndexElement);
            clusterIndexElement->SetText((unsigned int)line.ClusterIndex[j]);
        }
    }

    tinyxml2::XMLElement* pathNodesElement = a_doc->NewElement("Nodes");
    pathElement->InsertEndChild(pathNodesElement);

    const unsigned int pathNodeCount = a_model->GetPathNodeCount();
    const PathNodeCluster* pathNodes = a_model->GetPathNodes();
    for (unsigned int i = 0; i < pathNodeCount; ++i)
    {
        const PathNodeCluster& node = pathNodes[i];
        
        const int size = (int)node.Nodes.size();
        if (size > 0)
        {
            tinyxml2::XMLElement* pathNodeElement = a_doc->NewElement("Node");
            pathNodesElement->InsertEndChild(pathNodeElement);

            XMLIO::WriteVec3(a_doc, pathNodeElement, "Position", node.Nodes[0].Node.GetPosition());
            for (int j = 0; j < size; ++j)
            {
                const PathNode& pNode = node.Nodes[j];

                tinyxml2::XMLElement* clusterNodeElement = a_doc->NewElement("ClusterNode");
                pathNodeElement->InsertEndChild(clusterNodeElement);

                XMLIO::WriteVec3(a_doc, clusterNodeElement, "HandlePosition", pNode.Node.GetHandlePosition());

                tinyxml2::XMLElement* rotationElement = a_doc->NewElement("Rotation");
                clusterNodeElement->InsertEndChild(rotationElement);
                rotationElement->SetText(pNode.Rotation);

                XMLIO::WriteVec2(a_doc, clusterNodeElement, "Scale", pNode.Scale, glm::vec2(1.0f));

                const std::vector<BoneCluster> bones = pNode.Node.GetBones();

                if (bones.size() > 0)
                {
                    tinyxml2::XMLElement* bonesElement = a_doc->NewElement("Bones");
                    clusterNodeElement->InsertEndChild(bonesElement);

                    for (auto iter = bones.begin(); iter != bones.end(); ++iter)
                    {
                        tinyxml2::XMLElement* boneRootElement = a_doc->NewElement("Bone");
                        bonesElement->InsertEndChild(boneRootElement);

                        tinyxml2::XMLElement* boneIndexId = a_doc->NewElement("ID");
                        boneRootElement->InsertEndChild(boneIndexId);

                        boneIndexId->SetText(std::to_string(iter->ID).c_str());

                        if (iter->Weight > 0)
                        {
                            tinyxml2::XMLElement* weight = a_doc->NewElement("Weight");
                            boneRootElement->InsertEndChild(weight);

                            weight->SetText(iter->Weight);
                        }
                    }
                }
            }
        }   
    }
}
PathModel* PathModelSerializer::ParseData(Workspace* a_workspace, const tinyxml2::XMLElement* a_element, std::list<BoneGroup>* a_outBones)
{
    PathModel* model = new PathModel(a_workspace);

    model->m_armature = std::stoll(a_element->Attribute("Armature"));

    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();

        if (strcmp(str, "Shape") == 0)
        {
            model->SetShapeSteps(iter->IntAttribute("Steps", 1));

            unsigned int shapeLineCount;
            ShapeLine* shapeLines;
            unsigned int shapeNodeCount;
            ShapeNodeCluster* shapeNodes;

            for (const tinyxml2::XMLElement* sIter = iter->FirstChildElement(); sIter != nullptr; sIter = sIter->NextSiblingElement())
            {
                const char* sStr = sIter->Value();
                
                if (strcmp(sStr, "Lines") == 0)
                {
                    std::vector<ShapeLine> cLines;

                    for (const tinyxml2::XMLElement* lIter = sIter->FirstChildElement(); lIter != nullptr; lIter = lIter->NextSiblingElement())
                    {
                        const char* lStr = lIter->Value();

                        if (strcmp(lStr, "Line") == 0)
                        {
                            ShapeLine line;

                            int index = 0;
                            int cIndex = 0;

                            for (const tinyxml2::XMLElement* iIter = lIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                            {
                                const char* iStr = iIter->Value();
                                if (strcmp(iStr, "Index") == 0)
                                {
                                    line.Index[index++] = iIter->UnsignedText();
                                }
                                else if (strcmp(iStr, "ClusterIndex") == 0)
                                {
                                    line.ClusterIndex[cIndex++] = (unsigned char)iIter->UnsignedText();
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(iStr);
                                    printf("\n");
                                }
                            }

                            cLines.emplace_back(line);
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(lStr);
                            printf("\n");
                        }
                    }

                    shapeLineCount = (unsigned int)cLines.size();
                    shapeLines = new ShapeLine[shapeLineCount];

                    for (unsigned int i = 0; i < shapeLineCount; ++i)
                    {
                        shapeLines[i] = cLines[i];
                    }
                }
                else if (strcmp(sStr, "Nodes") == 0)
                {
                    std::vector<ShapeNodeCluster> cNodes;

                    for (const tinyxml2::XMLElement* nIter = sIter->FirstChildElement(); nIter != nullptr; nIter = nIter->NextSiblingElement())
                    {
                        const char* nStr = nIter->Value();
                        if (strcmp(nStr, "Node") == 0)
                        {
                            glm::vec2 pos = glm::vec2(0.0f);
                            ShapeNodeCluster c;

                            for (const tinyxml2::XMLElement* niIter = nIter->FirstChildElement(); niIter != nullptr; niIter = niIter->NextSiblingElement())
                            {
                                const char* niStr = niIter->Value();

                                if (strcmp(niStr, "Position") == 0)
                                {
                                    XMLIO::ReadVec2(niIter, &pos);
                                }
                                else if (strcmp(niStr, "ClusterNode") == 0)
                                {
                                    BezierCurveNode2 n;

                                    n.SetPosition(glm::vec2(std::numeric_limits<float>::infinity()));

                                    for (const tinyxml2::XMLElement* cIter = niIter->FirstChildElement(); cIter != nullptr; cIter = cIter->NextSiblingElement())
                                    {
                                        const char* cStr = cIter->Value();

                                        if (strcmp(cStr, "HandlePosition") == 0)
                                        {
                                            n.SetHandlePosition(XMLIO::GetVec2(cIter));
                                        }
                                        else
                                        {
                                            printf("PathModel::ParseData: Invalid Element: ");
                                            printf(cStr);
                                            printf("\n");
                                        }
                                    }

                                    c.Nodes.emplace_back(n);
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(niStr);
                                    printf("\n");
                                }
                            }

                            for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                            {
                                if (iter->GetPosition().x == std::numeric_limits<float>::infinity())
                                {
                                    iter->SetPosition(pos);
                                }
                            }

                            cNodes.emplace_back(c);
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(nStr);
                            printf("\n");
                        }
                    }

                    shapeNodeCount = (unsigned int)cNodes.size();
                    shapeNodes = new ShapeNodeCluster[shapeNodeCount];

                    for (unsigned int i = 0; i < shapeNodeCount; ++i)
                    {
                        shapeNodes[i] = cNodes[i];
                    }
                }   
                else
                {
                    printf("PathModel::ParseData: Invalid Element: ");
                    printf(sStr);
                    printf("\n");
                }
            }

            model->PassShapeModelData(shapeNodes, shapeNodeCount, shapeLines, shapeLineCount);
        }
        else if (strcmp(str, "Path") == 0)
        {
            model->SetPathSteps(iter->IntAttribute("Steps"));

            unsigned int pathLineCount;
            PathLine* pathLines;
            unsigned int pathNodeCount;
            PathNodeCluster* pathNodes;

            for (const tinyxml2::XMLElement* pIter = iter->FirstChildElement(); pIter != nullptr; pIter = pIter->NextSiblingElement())
            {
                const char* pStr = pIter->Value();

                if (strcmp(pStr, "Lines") == 0)
                {
                    std::vector<PathLine> cLines;

                    for (const tinyxml2::XMLElement* lIter = pIter->FirstChildElement(); lIter != nullptr; lIter = lIter->NextSiblingElement())
                    {
                        const char* lStr = lIter->Value();

                        if (strcmp(lStr, "Line") == 0)
                        {
                            PathLine line;

                            int index = 0;
                            int cIndex = 0;

                            for (const tinyxml2::XMLElement* iIter = lIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                            {
                                const char* iStr = iIter->Value();

                                if (strcmp(iStr, "Index") == 0)
                                {
                                    line.Index[index++] = iIter->UnsignedText();
                                }
                                else if (strcmp(iStr, "ClusterIndex") == 0)
                                {
                                    line.ClusterIndex[cIndex++] = (unsigned char)iIter->UnsignedText();
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(iStr);
                                    printf("\n");
                                }
                            }

                            cLines.emplace_back(line);
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(lStr);
                            printf("\n");
                        }
                    }

                    pathLineCount = (unsigned int)cLines.size();
                    pathLines = new PathLine[pathLineCount];

                    for (unsigned int i = 0; i < pathLineCount; ++i)
                    {
                        pathLines[i] = cLines[i];
                    }
                }
                else if (strcmp(pStr, "Nodes") == 0)
                {
                    std::vector<PathNodeCluster> cNodes;

                    for (const tinyxml2::XMLElement* nIter = pIter->FirstChildElement(); nIter != nullptr; nIter = nIter->NextSiblingElement())
                    {
                        const char* nStr = nIter->Value();

                        if (strcmp(nStr, "Node") == 0)
                        {
                            PathNodeCluster c;
                            glm::vec3 pos = glm::vec3(0.0f);

                            for (const tinyxml2::XMLElement* niIter = nIter->FirstChildElement(); niIter != nullptr; niIter = niIter->NextSiblingElement())
                            {
                                const char* niStr = niIter->Value();

                                if (strcmp(niStr, "Position") == 0)
                                {
                                    XMLIO::ReadVec3(niIter, &pos);
                                }
                                else if (strcmp(niStr, "ClusterNode") == 0)
                                {
                                    PathNode node;

                                    node.Node.SetPosition(glm::vec3(std::numeric_limits<float>::infinity()));

                                    for (const tinyxml2::XMLElement* cIter = niIter->FirstChildElement(); cIter != nullptr; cIter = cIter->NextSiblingElement())
                                    {
                                        const char* cStr = cIter->Value();

                                        if (strcmp(cStr, "HandlePosition") == 0)
                                        {
                                            node.Node.SetHandlePosition(XMLIO::GetVec3(cIter));
                                        }
                                        else if (strcmp(cStr, "Rotation") == 0)
                                        {
                                            node.Rotation = cIter->FloatText();   
                                        }
                                        else if (strcmp(cStr, "Scale") == 0)
                                        {
                                            node.Scale = XMLIO::GetVec2(cIter, glm::vec2(1.0f));
                                        }
                                        else if (strcmp(cStr, "Bones") == 0)
                                        {
                                            for (const tinyxml2::XMLElement* bIter = cIter->FirstChildElement(); bIter != nullptr; bIter = bIter->NextSiblingElement())
                                            {
                                                const char* bStr = bIter->Value();
                                                if (strcmp(bStr, "Bone") == 0)
                                                {
                                                    BoneGroup bone;
                                                    bone.Index = (unsigned int)cNodes.size();
                                                    bone.ClusterIndex = (unsigned int)c.Nodes.size();
                                                    bone.ID = 0;
                                                    bone.Weight = 0;

                                                    for (const tinyxml2::XMLElement* iBIter = bIter->FirstChildElement(); iBIter != nullptr; iBIter = iBIter->NextSiblingElement())
                                                    {
                                                        const char* iBStr = iBIter->Value();
                                                        if (strcmp(iBStr, "ID") == 0)
                                                        {
                                                            bone.ID = std::stoll(iBIter->GetText());
                                                        }
                                                        else if (strcmp(iBStr, "Weight") == 0)
                                                        {
                                                            bone.Weight = iBIter->FloatText();
                                                        }
                                                        else
                                                        {
                                                            printf("CurveModel::ParseData: InvalidElement: ");
                                                            printf(iBStr);
                                                            printf("\n");
                                                        }
                                                    }

                                                    a_outBones->emplace_back(bone);
                                                }
                                                else
                                                {
                                                    printf("CurveModel::ParseData: InvalidElement: ");
                                                    printf(bStr);
                                                    printf("\n");
                                                }
                                            }
                                        }
                                        else
                                        {
                                            printf("PathModel::ParseData: Invalid Element: ");
                                            printf(cStr);
                                            printf("\n");
                                        }
                                    }

                                    c.Nodes.emplace_back(node);
                                }
                                else
                                {
                                    printf("PathModel::ParseData: Invalid Element: ");
                                    printf(niStr);
                                    printf("\n");
                                }
                            }

                            for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                            {
                                if (iter->Node.GetPosition().x == std::numeric_limits<float>::infinity())
                                {
                                    iter->Node.SetPosition(pos);
                                }
                            }

                            cNodes.emplace_back(c);
                        }
                        else
                        {
                            printf("PathModel::ParseData: Invalid Element: ");
                            printf(nStr);
                            printf("\n");
                        }
                    }

                    pathNodeCount = (unsigned int)cNodes.size();
                    pathNodes = new PathNodeCluster[pathNodeCount];

                    for (unsigned int i = 0; i < pathNodeCount; ++i)
                    {
                        pathNodes[i] = cNodes[i];
                    }
                }
                else
                {
                    printf("PathModel::ParseData: Invalid Element: ");
                    printf(pStr);
                    printf("\n");
                }
            }

            model->PassPathModelData(pathNodes, pathNodeCount, pathLines, pathLineCount);
        }
        else
        {
            printf("PathModel::ParseData: Invalid Element: ");
            printf(str);
            printf("\n");
        }
    }

    return model;
}
void PathModelSerializer::PostParseData(PathModel* a_model, const std::list<BoneGroup>& a_bones, const std::unordered_map<long long, long long>& a_idMap)
{
    const auto armID = a_idMap.find(a_model->GetArmatureID());
    if (armID != a_idMap.end())
    {
        a_model->m_armature = armID->second;

        PathNodeCluster* nodes = a_model->GetPathNodes();
        for (auto iter = a_bones.begin(); iter != a_bones.end(); ++iter)
        {
            auto idIndex = a_idMap.find(iter->ID);
            if (idIndex != a_idMap.end())
            {
                nodes[iter->Index].Nodes[iter->ClusterIndex].Node.SetBoneWeight(idIndex->second, iter->Weight);
            }
        }
    }
    else
    {
        a_model->m_armature = -1;
    }
}