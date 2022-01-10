#include "MeshExporter.h"

#include "Model.h"

#include <string>

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