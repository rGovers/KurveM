#include "IO/MeshExporter.h"

#include <string>

#include "Model.h"
#include "Object.h"

void MeshExporter::SplitVertices(const Vertex* a_vertices, unsigned int a_vertexCount, 
    glm::vec4** a_positions, unsigned int* a_posCount, std::unordered_map<unsigned int, unsigned int>* a_posMap,
    glm::vec3** a_normals, unsigned int* a_normalCount, std::unordered_map<unsigned int, unsigned int>* a_normMap,
    glm::vec2** a_uvs, unsigned int* a_uvCount, std::unordered_map<unsigned int, unsigned int>* a_uvMap,
    glm::vec4** a_bones, glm::vec4** a_weights)
{
    *a_posCount = 0;
    *a_normalCount = 0;
    *a_uvCount = 0;

    *a_positions = new glm::vec4[a_vertexCount];
    *a_normals = new glm::vec3[a_vertexCount];
    *a_uvs = new glm::vec2[a_vertexCount];
    if (a_bones != nullptr)
    {
        *a_bones = new glm::vec4[a_vertexCount];
    }
    if (a_weights != nullptr)
    {
        *a_weights = new glm::vec4[a_vertexCount];
    }

    for (unsigned int i = 0; i < a_vertexCount; ++i)
    {
        const Vertex vert = a_vertices[i];

        for (unsigned int j = 0; j < *a_posCount; ++j)
        {
            const glm::vec4 diff = vert.Position - (*a_positions)[j];

            if (glm::dot(diff, diff) < 0.0001f)
            {
                a_posMap->emplace(i, j);

                goto PosFound;
            }
        }

        a_posMap->emplace(i, *a_posCount);
        (*a_positions)[*a_posCount] = vert.Position;
        if (a_bones != nullptr)
        {
            (*a_bones)[*a_posCount] = vert.Bones;
        }
        if (a_weights != nullptr)
        {
            (*a_weights)[*a_posCount] = vert.Weights;
        }
        ++*a_posCount;
PosFound:;

        for (unsigned int j = 0; j < *a_normalCount; ++j)
        {
            const glm::vec3 diff = vert.Normal - (*a_normals)[j];
            if (glm::dot(diff, diff) < 0.0001f)
            {
                a_normMap->emplace(i, j);

                goto NormFound;
            }
        }

        a_normMap->emplace(i, *a_normalCount);
        (*a_normals)[*a_normalCount] = vert.Normal;
        ++*a_normalCount;
NormFound:;

        for (unsigned int j = 0; j < *a_uvCount; ++j)
        {
            const glm::vec2 diff = vert.UV - (*a_uvs)[j];
            if (glm::dot(diff, diff) < 0.0001f)
            {
                a_uvMap->emplace(i, j);

                goto UVFound;
            }
        }

        a_uvMap->emplace(i, *a_uvCount);
        (*a_uvs)[*a_uvCount] = vert.UV;
        ++*a_uvCount;
UVFound:;
    }
}

void MeshExporter::ExportOBJMesh(std::ofstream* a_file, const Vertex* a_vertices, unsigned int a_vertexCount, const unsigned int* a_indices, unsigned int a_indexCount)
{
    a_file->write("\n", 1);

    const char* vertexPosComment = "# Vertex Data \n";
    a_file->write(vertexPosComment, strlen(vertexPosComment));

    glm::vec4* posVerts;
    glm::vec3* normVerts;
    glm::vec2* uvVerts;

    unsigned int posCount;
    unsigned int normCount;
    unsigned int uvCount;

    std::unordered_map<unsigned int, unsigned int> posMap;
    std::unordered_map<unsigned int, unsigned int> normMap;
    std::unordered_map<unsigned int, unsigned int> uvMap;

    SplitVertices(a_vertices, a_vertexCount, &posVerts, &posCount, &posMap, &normVerts, &normCount, &normMap, &uvVerts, &uvCount, &uvMap);

    for (unsigned int i = 0; i < posCount; ++i)
    {
        const glm::vec4 pos = posVerts[i];

        a_file->write("v", 1);
        for (int j = 0; j < 4; ++j)
        {
            const std::string str = " " + std::to_string(pos[j]);
            a_file->write(str.c_str(), str.length());
        }
        
        a_file->write("\n", 1);
    }

    a_file->write("\n", 1);

    for (unsigned int i = 0; i < normCount; ++i)
    {
        const glm::vec3 norm = -normVerts[i];

        a_file->write("vn", 2);
        for (int j = 0; j < 3; ++j)
        {
            const std::string str = " " + std::to_string(norm[j]);
            a_file->write(str.c_str(), str.length());
        }

        a_file->write("\n", 1);
    }

    a_file->write("\n", 1);

    for (unsigned int i = 0; i < uvCount; ++i)
    {
        const glm::vec2 uv = uvVerts[i];

        a_file->write("vt", 2);
        for (int j = 0; j < 2; ++j)
        {
            const std::string str = " " + std::to_string(uv[j]);
            a_file->write(str.c_str(), str.length());
        }

        a_file->write("\n", 1);
    }

    a_file->write("\n", 1);

    const char* facesComment = "# Index Data \n";
    a_file->write(facesComment, strlen(facesComment));

    for (unsigned int i = 0; i < a_indexCount; i += 3)
    {
        a_file->write("f ", 2);
        
        for (int j = 0; j < 3; ++j)
        {
            const unsigned int index = a_indices[i + j];

            const std::string pStr = std::to_string(posMap[index] + 1);
            const std::string nStr = std::to_string(normMap[index] + 1);
            const std::string uStr = std::to_string(uvMap[index] + 1);

            const std::string str = pStr + "/" + uStr + '/' + nStr + " ";

            a_file->write(str.c_str(), str.length());
        }

        a_file->write("\n", 1);    
    }

    delete[] posVerts;
    delete[] normVerts;
    delete[] uvVerts;
}

void MeshExporter::ExportColladaMesh(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, 
    const Vertex* a_vertices, unsigned int a_vertexCount, const unsigned int* a_indices, unsigned int a_indexCount, 
    const char* a_name)
{
    glm::vec4* posVerts;
    glm::vec3* normVerts;
    glm::vec2* uvVerts;

    unsigned int posCount;
    unsigned int normCount;
    unsigned int uvCount;

    std::unordered_map<unsigned int, unsigned int> posMap;
    std::unordered_map<unsigned int, unsigned int> normMap;
    std::unordered_map<unsigned int, unsigned int> uvMap;

    SplitVertices(a_vertices, a_vertexCount, &posVerts, &posCount, &posMap, &normVerts, &normCount, &normMap, &uvVerts, &uvCount, &uvMap);

    ExportColladaMesh(a_doc, a_parent, posVerts, posCount, normVerts, normCount, uvVerts, uvCount, nullptr, nullptr, posMap, normMap, uvMap, a_indices, a_indexCount, a_name);

    delete[] posVerts;
    delete[] normVerts;
    delete[] uvVerts;
}
void MeshExporter::ExportColladaMesh(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, 
    const glm::vec4* a_posVerts, unsigned int a_posVertCount, 
    const glm::vec3* a_normVerts, unsigned int a_normVertCount, 
    const glm::vec2* a_uvVerts, unsigned int a_uvVertCount, 
    const glm::vec4* a_bones, const glm::vec4* a_weights, 
    const std::unordered_map<unsigned int, unsigned int>& a_posMap, const std::unordered_map<unsigned int, unsigned int>& a_normMap, const std::unordered_map<unsigned int, unsigned int>& a_uvMap, 
    const unsigned int* a_indices, unsigned int a_indexCount, 
    const char* a_name)
{
    // Position
    const std::string posSourceStr = std::string(a_name) + "-pos";

    tinyxml2::XMLElement* posSourceElement = a_doc->NewElement("source");
    a_parent->InsertEndChild(posSourceElement);
    posSourceElement->SetAttribute("id", posSourceStr.c_str());

    const std::string posFloatArrayStr = posSourceStr + "-array";

    tinyxml2::XMLElement* posFloatArray = a_doc->NewElement("float_array");
    posSourceElement->InsertEndChild(posFloatArray);
    posFloatArray->SetAttribute("id", posFloatArrayStr.c_str());
    posFloatArray->SetAttribute("count", a_posVertCount * 3);
    
    tinyxml2::XMLElement* posTechniqueElement = a_doc->NewElement("technique_common");
    posSourceElement->InsertEndChild(posTechniqueElement);

    tinyxml2::XMLElement* posAccessorElement = a_doc->NewElement("accessor");
    posTechniqueElement->InsertEndChild(posAccessorElement);
    posAccessorElement->SetAttribute("source", ("#" + posFloatArrayStr).c_str());
    posAccessorElement->SetAttribute("count", a_posVertCount);
    posAccessorElement->SetAttribute("stride", 3);

    tinyxml2::XMLElement* posParamXElement = a_doc->NewElement("param");
    posAccessorElement->InsertEndChild(posParamXElement);
    posParamXElement->SetAttribute("name", "X");
    posParamXElement->SetAttribute("type", "float");

    tinyxml2::XMLElement* posParamYElement = a_doc->NewElement("param");
    posAccessorElement->InsertEndChild(posParamYElement);
    posParamYElement->SetAttribute("name", "Y");
    posParamYElement->SetAttribute("type", "float");

    tinyxml2::XMLElement* posParamZElement = a_doc->NewElement("param");
    posAccessorElement->InsertEndChild(posParamZElement);
    posParamZElement->SetAttribute("name", "Z");
    posParamZElement->SetAttribute("type", "float");

    std::string pData;

    for (unsigned int i = 0; i < a_posVertCount; ++i)
    {
        const glm::vec3 pos = a_posVerts[i];

        pData += std::to_string(pos.x);
        pData += " ";
        pData += std::to_string(-pos.y);
        pData += " ";
        pData += std::to_string(pos.z);
        pData += "\n";
    }

    posFloatArray->SetText(pData.c_str());

    // Normal
    const std::string normSourceStr = std::string(a_name) + "-norm";

    tinyxml2::XMLElement* normalSourceElement = a_doc->NewElement("source");
    a_parent->InsertEndChild(normalSourceElement);
    normalSourceElement->SetAttribute("id", normSourceStr.c_str());

    const std::string normFloatArrayStr = normSourceStr + "-array";

    tinyxml2::XMLElement* normFloatArray = a_doc->NewElement("float_array");
    normalSourceElement->InsertEndChild(normFloatArray);
    normFloatArray->SetAttribute("id", normFloatArrayStr.c_str());
    normFloatArray->SetAttribute("count", a_normVertCount * 3);
    
    tinyxml2::XMLElement* normTechniqueElement = a_doc->NewElement("technique_common");
    normalSourceElement->InsertEndChild(normTechniqueElement);

    tinyxml2::XMLElement* normAccessorElement = a_doc->NewElement("accessor");
    normTechniqueElement->InsertEndChild(normAccessorElement);
    normAccessorElement->SetAttribute("source", ("#" + normFloatArrayStr).c_str());
    normAccessorElement->SetAttribute("count", a_normVertCount);
    normAccessorElement->SetAttribute("stride", 3);

    tinyxml2::XMLElement* normParamXElement = a_doc->NewElement("param");
    normAccessorElement->InsertEndChild(normParamXElement);
    normParamXElement->SetAttribute("name", "X");
    normParamXElement->SetAttribute("type", "float");

    tinyxml2::XMLElement* normParamYElement = a_doc->NewElement("param");
    normAccessorElement->InsertEndChild(normParamYElement);
    normParamYElement->SetAttribute("name", "Y");
    normParamYElement->SetAttribute("type", "float");

    tinyxml2::XMLElement* normParamZElement = a_doc->NewElement("param");
    normAccessorElement->InsertEndChild(normParamZElement);
    normParamZElement->SetAttribute("name", "Z");
    normParamZElement->SetAttribute("type", "float");

    std::string nData;

    for (unsigned int i = 0; i < a_normVertCount; ++i)
    {
        const glm::vec3 normal = a_normVerts[i];

        nData += std::to_string(-normal.x);
        nData += " ";
        nData += std::to_string(normal.y);
        nData += " ";
        nData += std::to_string(-normal.z);
        nData += "\n";
    }

    normFloatArray->SetText(nData.c_str());

    // UV
    const std::string texCoordSourceStr = std::string(a_name) + "-texcoords";

    tinyxml2::XMLElement* uvSourceElement = a_doc->NewElement("source");
    a_parent->InsertEndChild(uvSourceElement);
    uvSourceElement->SetAttribute("id", texCoordSourceStr.c_str());

    const std::string uvFloatArrayStr = texCoordSourceStr + "-array";

    tinyxml2::XMLElement* uvFloatArray = a_doc->NewElement("float_array");
    uvSourceElement->InsertEndChild(uvFloatArray);
    uvFloatArray->SetAttribute("id", uvFloatArrayStr.c_str());
    uvFloatArray->SetAttribute("count", a_uvVertCount * 2);
    
    tinyxml2::XMLElement* uvTechniqueElement = a_doc->NewElement("technique_common");
    uvSourceElement->InsertEndChild(uvTechniqueElement);

    tinyxml2::XMLElement* uvAccessorElement = a_doc->NewElement("accessor");
    uvTechniqueElement->InsertEndChild(uvAccessorElement);
    uvAccessorElement->SetAttribute("source", ("#" + uvFloatArrayStr).c_str());
    uvAccessorElement->SetAttribute("count", a_uvVertCount);
    uvAccessorElement->SetAttribute("stride", 2);

    tinyxml2::XMLElement* uvParamXElement = a_doc->NewElement("param");
    uvAccessorElement->InsertEndChild(uvParamXElement);
    uvParamXElement->SetAttribute("name", "X");
    uvParamXElement->SetAttribute("type", "float");

    tinyxml2::XMLElement* uvParamYElement = a_doc->NewElement("param");
    uvAccessorElement->InsertEndChild(uvParamYElement);
    uvParamYElement->SetAttribute("name", "Y");
    uvParamYElement->SetAttribute("type", "float");

    std::string uData;

    for (unsigned int i = 0; i < a_uvVertCount; ++i)
    {
        const glm::vec2 uv = a_uvVerts[i];

        for (int j = 0; j < 2; ++j)
        {
            uData += std::to_string(uv[j]);
            uData += " ";
        }
        uData += "\n";
    }

    uvFloatArray->SetText(uData.c_str());

    const std::string vertexStr = std::string(a_name) + "-vtx";

    tinyxml2::XMLElement* verticesElement = a_doc->NewElement("vertices");
    a_parent->InsertEndChild(verticesElement);
    verticesElement->SetAttribute("id", vertexStr.c_str());

    tinyxml2::XMLElement* inputVertexElement = a_doc->NewElement("input");
    verticesElement->InsertEndChild(inputVertexElement);
    inputVertexElement->SetAttribute("semantic", "POSITION");
    inputVertexElement->SetAttribute("source", ("#" + posSourceStr).c_str());

    tinyxml2::XMLElement* polygonsElement = a_doc->NewElement("polygons");
    a_parent->InsertEndChild(polygonsElement);
    polygonsElement->SetAttribute("count", a_indexCount / 3);

    tinyxml2::XMLElement* polyInputVertexElement = a_doc->NewElement("input");
    polygonsElement->InsertEndChild(polyInputVertexElement);
    polyInputVertexElement->SetAttribute("semantic", "VERTEX");
    polyInputVertexElement->SetAttribute("source", ("#" + vertexStr).c_str());
    polyInputVertexElement->SetAttribute("offset", 0);

    tinyxml2::XMLElement* polyInputNormalElement = a_doc->NewElement("input");
    polygonsElement->InsertEndChild(polyInputNormalElement);
    polyInputNormalElement->SetAttribute("semantic", "NORMAL");
    polyInputNormalElement->SetAttribute("source", ("#" + normSourceStr).c_str());
    polyInputNormalElement->SetAttribute("offset", 1);

    tinyxml2::XMLElement* polyInputTexcoordElement = a_doc->NewElement("input");
    polygonsElement->InsertEndChild(polyInputTexcoordElement);
    polyInputTexcoordElement->SetAttribute("semantic", "TEXCOORD");
    polyInputTexcoordElement->SetAttribute("source", ("#" + texCoordSourceStr).c_str());
    polyInputTexcoordElement->SetAttribute("offset", 2);

    const int indexDat[3] = { 1, 0, 2 };

    for (unsigned int i = 0; i < a_indexCount; i += 3)
    {
        std::string pStr;
        for (int j = 0; j < 3; ++j)
        {
            const unsigned int index = a_indices[i + indexDat[j]];
            pStr += std::to_string(a_posMap.at(index));
            pStr += " ";
            pStr += std::to_string(a_normMap.at(index));
            pStr += " ";
            pStr += std::to_string(a_uvMap.at(index));
            pStr += " ";
        }

        tinyxml2::XMLElement* pElement = a_doc->NewElement("p");
        polygonsElement->InsertEndChild(pElement);
        pElement->SetText(pStr.c_str());
    }
}

void MeshExporter::ExportColladaBones(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parentController, const glm::vec4* a_bones, const glm::vec4* a_weights, unsigned int a_vertCount, const char* a_parentID, const char* a_name, const std::list<Object*> a_armNodes, char** a_outRoot)
{
    const unsigned int jointCount = a_armNodes.size();
    const unsigned int vertDataSize = a_vertCount * 4;

    glm::mat4* invBind = new glm::mat4[jointCount];

    tinyxml2::XMLElement* skinElement = a_doc->NewElement("skin");
    a_parentController->InsertEndChild(skinElement);
    skinElement->SetAttribute("source", ("#" + std::string(a_parentID)).c_str());

    const std::string jointsName = std::string(a_name) + "-joints";

    tinyxml2::XMLElement* jointSourceElement = a_doc->NewElement("source");
    skinElement->InsertEndChild(jointSourceElement);
    jointSourceElement->SetAttribute("id", jointsName.c_str());

    const std::string jointArrayName = jointsName + "-array";

    tinyxml2::XMLElement* jointSourceArrayElement = a_doc->NewElement("Name_array");
    jointSourceElement->InsertEndChild(jointSourceArrayElement);
    jointSourceArrayElement->SetAttribute("id", jointArrayName.c_str());
    jointSourceArrayElement->SetAttribute("count", jointCount);

    tinyxml2::XMLElement* jointTechniqueElement = a_doc->NewElement("technique_common");
    jointSourceElement->InsertEndChild(jointTechniqueElement);

    tinyxml2::XMLElement* jointAccessorElement = a_doc->NewElement("accessor");
    jointTechniqueElement->InsertEndChild(jointAccessorElement);
    jointAccessorElement->SetAttribute("source", ("#" + jointArrayName).c_str());
    jointAccessorElement->SetAttribute("count", jointCount);
    jointAccessorElement->SetAttribute("stride", 1);

    tinyxml2::XMLElement* jointParamElement = a_doc->NewElement("param");
    jointAccessorElement->InsertEndChild(jointParamElement);
    jointParamElement->SetAttribute("name", "JOINT");
    jointParamElement->SetAttribute("type", "name");

    std::string jData;

    *a_outRoot = (*a_armNodes.begin())->GetIDName();

    unsigned int index = 0;
    for (auto iter = a_armNodes.begin(); iter != a_armNodes.end(); ++iter)
    {
        const Object* obj = *iter;

        const char* name = obj->GetIDName();

        jData += name;
        jData += "\n";

        // I think? It has been a while need to double check
        invBind[index++] = glm::inverse(obj->GetGlobalMatrix());

        delete[] name;
    }

    jointSourceArrayElement->SetText(jData.c_str());

    const std::string weightName = std::string(a_name) + "-weights";

    tinyxml2::XMLElement* weightSourceElement = a_doc->NewElement("source");
    skinElement->InsertEndChild(weightSourceElement);
    weightSourceElement->SetAttribute("id", weightName.c_str());

    const std::string weightArrayName = weightName + "-array";

    tinyxml2::XMLElement* weightSourceArray = a_doc->NewElement("float_array");
    weightSourceElement->InsertEndChild(weightSourceArray);
    weightSourceArray->SetAttribute("id", weightArrayName.c_str());
    weightSourceArray->SetAttribute("count", vertDataSize);

    tinyxml2::XMLElement* weightTechniqueElement = a_doc->NewElement("technique_common");
    weightSourceElement->InsertEndChild(weightTechniqueElement);

    tinyxml2::XMLElement* weightAccessorElement = a_doc->NewElement("accessor");
    weightTechniqueElement->InsertEndChild(weightAccessorElement);
    weightAccessorElement->SetAttribute("source", ("#" + weightArrayName).c_str());
    weightAccessorElement->SetAttribute("count", vertDataSize);
    weightAccessorElement->SetAttribute("stride", 1);

    tinyxml2::XMLElement* weightParamElement = a_doc->NewElement("param");
    weightAccessorElement->InsertEndChild(weightParamElement);
    weightParamElement->SetAttribute("name", "WEIGHT");
    weightParamElement->SetAttribute("type", "float");

    std::string wData;

    for (unsigned int i = 0; i < a_vertCount; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            wData += std::to_string(a_weights[i][j]);
            wData += " ";
        }
        wData += "\n";
    }

    weightSourceArray->SetText(wData.c_str());

    const std::string invBindName = std::string(a_name) + "-invbind";

    tinyxml2::XMLElement* invBindSourceElement = a_doc->NewElement("source");
    skinElement->InsertEndChild(invBindSourceElement);
    invBindSourceElement->SetAttribute("id", invBindName.c_str());

    const std::string invBindArrayName = invBindName + "-array";

    tinyxml2::XMLElement* invBindSourceArrayElement = a_doc->NewElement("float_array");
    invBindSourceElement->InsertEndChild(invBindSourceArrayElement);
    invBindSourceArrayElement->SetAttribute("id", invBindArrayName.c_str());
    invBindSourceArrayElement->SetAttribute("count", jointCount * 16);

    tinyxml2::XMLElement* invBindTechniqueElement = a_doc->NewElement("technique_common");
    invBindSourceElement->InsertEndChild(invBindTechniqueElement);

    tinyxml2::XMLElement* invBindAccessorElement = a_doc->NewElement("accessor");
    invBindTechniqueElement->InsertEndChild(invBindAccessorElement);
    invBindAccessorElement->SetAttribute("source", ("#" + invBindArrayName).c_str());
    invBindAccessorElement->SetAttribute("count", jointCount);
    invBindAccessorElement->SetAttribute("stride", 16);

    tinyxml2::XMLElement* invBindParamElement = a_doc->NewElement("param");
    invBindAccessorElement->InsertEndChild(invBindParamElement);
    invBindParamElement->SetAttribute("name", "TRANSFORM");
    invBindParamElement->SetAttribute("type", "float4x4");

    std::string iBData;

    for (unsigned int i = 0; i < jointCount; ++i)
    {
        const glm::mat4 mat = invBind[i];

        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                iBData += std::to_string(mat[j][k]);
                iBData += " ";
            }
        }
        iBData += "\n";
    }

    invBindSourceArrayElement->SetText(iBData.c_str());

    tinyxml2::XMLElement* jointsElement = a_doc->NewElement("joints");
    skinElement->InsertEndChild(jointsElement);

    tinyxml2::XMLElement* jointJointInputElement = a_doc->NewElement("input");
    jointsElement->InsertEndChild(jointJointInputElement);
    jointJointInputElement->SetAttribute("semantic", "JOINT");
    jointJointInputElement->SetAttribute("source", ("#" + jointsName).c_str());

    tinyxml2::XMLElement* jointInvBindInputElement = a_doc->NewElement("input");
    jointsElement->InsertEndChild(jointInvBindInputElement);
    jointInvBindInputElement->SetAttribute("semantic", "INV_BIND_MATRIX");
    jointInvBindInputElement->SetAttribute("source", ("#" + invBindName).c_str());

    tinyxml2::XMLElement* vertexWeightElement = a_doc->NewElement("vertex_weights");
    skinElement->InsertEndChild(vertexWeightElement);
    vertexWeightElement->SetAttribute("count", a_vertCount);

    tinyxml2::XMLElement* vertexWeightJointInputElement = a_doc->NewElement("input");
    vertexWeightElement->InsertEndChild(vertexWeightJointInputElement);
    vertexWeightJointInputElement->SetAttribute("semantic", "JOINT");
    vertexWeightJointInputElement->SetAttribute("source", ("#" + jointsName).c_str());
    vertexWeightJointInputElement->SetAttribute("offset", 0);

    tinyxml2::XMLElement* vertexWeightWeightInputElement = a_doc->NewElement("input");
    vertexWeightElement->InsertEndChild(vertexWeightWeightInputElement);
    vertexWeightWeightInputElement->SetAttribute("semantic", "WEIGHT");
    vertexWeightWeightInputElement->SetAttribute("source", ("#" + weightName).c_str());
    vertexWeightWeightInputElement->SetAttribute("offset", 1);

    tinyxml2::XMLElement* vCountElement = a_doc->NewElement("vcount");
    vertexWeightElement->InsertEndChild(vCountElement);

    tinyxml2::XMLElement* vElement = a_doc->NewElement("v");
    vertexWeightElement->InsertEndChild(vElement);

    std::string vData;
    std::string vCData;

    for (unsigned int i = 0; i < a_vertCount; ++i)
    {
        vCData += std::to_string(4);
        vCData += "\n";

        for (int j = 0; j < 4; ++j)
        {
            vData += std::to_string((unsigned int)(a_bones[i][j] * jointCount));
            vData += " ";
            vData += std::to_string(i * 4 + j);
            vData += " ";
        }

        vData += "\n";
    }

    vCountElement->SetText(vCData.c_str());
    vElement->SetText(vData.c_str());

    delete[] invBind;
}