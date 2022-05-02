#include "IO/CurveModelSerializer.h"

#include "CurveModel.h"
#include "IO/MeshExporter.h"
#include "IO/ObjectSerializer.h"
#include "IO/XMLIO.h"
#include "Model.h"
#include "Object.h"

void CurveModelSerializer::WriteOBJ(std::ofstream* a_file, const CurveModel* a_model, bool a_stepAdjust, int a_steps)
{
    Vertex* vertices;
    unsigned int* indices;

    unsigned int indexCount;
    unsigned int vertexCount;

    a_model->GetModelData(a_stepAdjust, a_steps, &indices, &indexCount, &vertices, &vertexCount);

    MeshExporter::ExportOBJMesh(a_file, vertices, vertexCount, indices, indexCount);

    delete[] vertices;
    delete[] indices;
}
void CurveModelSerializer::WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, tinyxml2::XMLElement* a_parentController, const CurveModel* a_model, const char* a_parentID, const char* a_name, bool a_stepAdjust, int a_steps, char** a_outRoot)
{
    tinyxml2::XMLElement* meshElement = a_doc->NewElement("mesh");
    a_parent->InsertEndChild(meshElement);

    Vertex* vertices;
    unsigned int* indices; 

    unsigned int indexCount;
    unsigned int vertexCount;

    a_model->GetModelData(a_stepAdjust, a_steps, &indices, &indexCount, &vertices, &vertexCount);

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

void CurveModelSerializer::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const CurveModel* a_model)
{
    tinyxml2::XMLElement* curveModelElement = a_doc->NewElement("CurveModel");
    a_parent->InsertEndChild(curveModelElement);

    curveModelElement->SetAttribute("StepAdjust", a_model->IsStepAdjusted());
    curveModelElement->SetAttribute("Steps", a_model->GetSteps());
    curveModelElement->SetAttribute("Armature", std::to_string(a_model->GetArmatureID()).c_str());

    tinyxml2::XMLElement* facesElement = a_doc->NewElement("Faces");
    curveModelElement->InsertEndChild(facesElement);

    const unsigned int faceCount = a_model->GetFaceCount();
    const CurveFace* faces = a_model->GetFaces();
    for (unsigned int i = 0; i < faceCount; ++i)
    {
        const CurveFace& face = faces[i];

        tinyxml2::XMLElement* fElement = a_doc->NewElement("Face");
        facesElement->InsertEndChild(fElement);

        fElement->SetAttribute("FaceMode", (int)face.FaceMode);

        switch (face.FaceMode)
        {
        case FaceMode_3Point:
        {
            for (int i = 0; i < 6; ++i)
            {
                tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
                fElement->InsertEndChild(indexElement);
                indexElement->SetText(face.Index[i]);

                tinyxml2::XMLElement* clusterIndexElement = a_doc->NewElement("ClusterIndex");
                fElement->InsertEndChild(clusterIndexElement);
                clusterIndexElement->SetText(face.ClusterIndex[i]);
            }

            break;
        }
        case FaceMode_4Point:
        {
            for (int i = 0; i < 8; ++i)
            {
                tinyxml2::XMLElement* indexElement = a_doc->NewElement("Index");
                fElement->InsertEndChild(indexElement);
                indexElement->SetText(face.Index[i]);

                tinyxml2::XMLElement* clusterIndexElement = a_doc->NewElement("ClusterIndex");
                fElement->InsertEndChild(clusterIndexElement);
                clusterIndexElement->SetText(face.ClusterIndex[i]);
            }

            break;
        }
        }
    }

    tinyxml2::XMLElement* nodesElement = a_doc->NewElement("Nodes");
    curveModelElement->InsertEndChild(nodesElement);

    const unsigned int nodeCount = a_model->GetNodeCount();
    const CurveNodeCluster* nodes = a_model->GetNodes();
    for (unsigned int i = 0; i < nodeCount; ++i)
    {
        const CurveNodeCluster& node = nodes[i];

        tinyxml2::XMLElement* nElement = a_doc->NewElement("Node");
        nodesElement->InsertEndChild(nElement);

        const std::vector<CurveNode>& nodes = node.Nodes;
        const int size = (int)nodes.size();

        if (size > 0)
        {
            XMLIO::WriteVec3(a_doc, nElement, "Position", nodes[0].Node.GetPosition());

            for (int j = 0; j < size; ++j)
            {
                const CurveNode& g = nodes[j];

                tinyxml2::XMLElement* cNodeElement = a_doc->NewElement("ClusterNode");
                nElement->InsertEndChild(cNodeElement);

                XMLIO::WriteVec3(a_doc, cNodeElement, "HandlePosition", g.Node.GetHandlePosition());
                XMLIO::WriteVec2(a_doc, cNodeElement, "UV", g.Node.GetUV());

                const std::vector<BoneCluster> bones = g.Node.GetBones();

                if (bones.size() > 0)
                {
                    tinyxml2::XMLElement* bonesElement = a_doc->NewElement("Bones");
                    cNodeElement->InsertEndChild(bonesElement);

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
CurveModel* CurveModelSerializer::ParseData(Workspace* a_workspace, const tinyxml2::XMLElement* a_element, std::list<BoneGroup>* a_outBones)
{   
    constexpr float infinity = std::numeric_limits<float>::infinity();
    constexpr glm::vec3 infinity3 = glm::vec3(infinity);

    CurveModel* model = new CurveModel(a_workspace);

    model->SetStepAdjust(a_element->BoolAttribute("StepAdjust"));
    model->SetSteps(a_element->IntAttribute("Steps"));
    model->m_armature = std::stoll(a_element->Attribute("Armature"));

    CurveFace* faces;
    unsigned int faceCount;
    CurveNodeCluster* nodes;
    unsigned int nodeCount;

    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();

        if (strcmp(str, "Faces") == 0)
        {
            std::vector<CurveFace> cFaces;

            for (const tinyxml2::XMLElement* fIter = iter->FirstChildElement(); fIter != nullptr; fIter = fIter->NextSiblingElement())
            {
                const char* fStr = fIter->Value();

                if (strcmp(fStr, "Face") == 0)
                {
                    CurveFace face;

                    face.FaceMode = (e_FaceMode)fIter->IntAttribute("FaceMode");

                    int index = 0;
                    int cIndex = 0;
                    for (const tinyxml2::XMLElement* iIter = fIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                    {
                        const char* iStr = iIter->Value();

                        if (strcmp(iStr, "Index") == 0)
                        {
                            face.Index[index++] = iIter->UnsignedText();
                        }
                        else if (strcmp(iStr, "ClusterIndex") == 0)
                        {
                            face.ClusterIndex[cIndex++] = iIter->UnsignedText();
                        }
                        else 
                        {
                            printf("CurveModel::ParseData: InvalidElement: ");
                            printf(iStr);
                            printf("\n");
                        }
                    }

                    cFaces.emplace_back(face);
                }
                else 
                {
                    printf("CurveModel::ParseData: Invalid Element: ");
                    printf(fStr);
                    printf("\n");
                }
            }

            faceCount = (unsigned int)cFaces.size();

            faces = new CurveFace[faceCount];

            for (unsigned int i = 0; i < faceCount; ++i)
            {
                faces[i] = cFaces[i];
            }
        }
        else if (strcmp(str, "Nodes") == 0)
        {
            std::vector<CurveNodeCluster> cNodes;

            for (const tinyxml2::XMLElement* nIter = iter->FirstChildElement(); nIter != nullptr; nIter = nIter->NextSiblingElement())
            {
                const char* nStr = nIter->Value();

                if (strcmp(nStr, "Node") == 0)
                {
                    CurveNodeCluster node;

                    glm::vec3 pos = glm::vec3(0.0f);

                    for (const tinyxml2::XMLElement* iIter = nIter->FirstChildElement(); iIter != nullptr; iIter = iIter->NextSiblingElement())
                    {
                        const char* iStr = iIter->Value();

                        if (strcmp(iStr, "Position") == 0)
                        {
                            XMLIO::ReadVec3(iIter, &pos);
                        }
                        else if (strcmp(iStr, "ClusterNode") == 0)
                        {
                            CurveNode n;

                            n.Node.SetPosition(infinity3);

                            for (const tinyxml2::XMLElement* cIter = iIter->FirstChildElement(); cIter != nullptr; cIter = cIter->NextSiblingElement())
                            {
                                const char* cStr = cIter->Value();

                                if (strcmp(cStr, "HandlePosition") == 0)
                                {
                                    n.Node.SetHandlePosition(XMLIO::GetVec3(cIter));
                                }
                                else if (strcmp(cStr, "UV") == 0)
                                {
                                    n.Node.SetUV(XMLIO::GetVec2(cIter));
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
                                            bone.ClusterIndex = (unsigned int)node.Nodes.size();
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
                                    printf("CurveModel::ParseData: InvalidElement: ");
                                    printf(cStr);
                                    printf("\n");
                                }
                            }

                            node.Nodes.emplace_back(n);
                        }
                        else
                        {
                            printf("CurveModel::ParseData: InvalidElement: ");
                            printf(iStr);
                            printf("\n");
                        }
                    }

                    for (auto iter = node.Nodes.begin(); iter != node.Nodes.end(); ++iter)
                    {
                        if (iter->Node.GetPosition().x == infinity)
                        {
                            iter->Node.SetPosition(pos);
                        }
                    }

                    cNodes.emplace_back(node);
                }
                else
                {
                    printf("CurveModel::ParseData: Invalid Element: ");
                    printf(nStr);
                    printf("\n");
                }
            }

            nodeCount = (unsigned int)cNodes.size();

            nodes = new CurveNodeCluster[nodeCount];

            for (unsigned int i = 0; i < nodeCount; ++i)
            {
                nodes[i] = cNodes[i];
            }
        }
        else
        {
            printf("CurveModel::ParseData: Invalid Element: ");
            printf(str);
            printf("\n");
        }
    }

    for (unsigned int i = 0; i < faceCount; ++i)
    {
        const CurveFace& face = faces[i];

        switch (face.FaceMode)
        {
        case FaceMode_3Point:
        {
            for (int i = 0; i < 6; ++i)
            {
                ++nodes[face.Index[i]].Nodes[face.ClusterIndex[i]].FaceCount;
            }

            break;
        }
        case FaceMode_4Point:
        {
            for (int i = 0; i < 8; ++i)
            {
                ++nodes[face.Index[i]].Nodes[face.ClusterIndex[i]].FaceCount;
            }

            break;
        }
        }
    }

    model->PassModelData(nodes, nodeCount, faces, faceCount);

    return model;
}
void CurveModelSerializer::PostParseData(CurveModel* a_model, const std::list<BoneGroup>& a_bones, const std::unordered_map<long long, long long>& a_idMap)
{
    const auto armID = a_idMap.find(a_model->GetArmatureID());
    if (armID != a_idMap.end())
    {
        a_model->m_armature = armID->second;

        CurveNodeCluster* nodes = a_model->GetNodes();
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