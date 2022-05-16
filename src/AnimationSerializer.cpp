#include "IO/AnimationSerializer.h"

#include <string>

#include "Animation.h"
#include "IO/XMLIO.h"
#include "Object.h"
#include "Workspace.h"

void AnimationSerializer::CreateColladaSampler(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_element)
{
    tinyxml2::XMLElement* samplerElement = a_doc->NewElement("sampler");
    a_parent->InsertEndChild(samplerElement);
    const std::string samplerID = std::string(a_element) + "-sampler";
    samplerElement->SetAttribute("id", samplerID.c_str());

    tinyxml2::XMLElement* inputElement = a_doc->NewElement("input");
    samplerElement->InsertEndChild(inputElement);
    inputElement->SetAttribute("semantic", "INPUT");
    inputElement->SetAttribute("source", "#time-TIME-source");

    tinyxml2::XMLElement* outputElement = a_doc->NewElement("input");
    samplerElement->InsertEndChild(outputElement);
    const std::string outputStr = std::string("#") + a_element + "-source";
    outputElement->SetAttribute("semantic", "OUTPUT");
    outputElement->SetAttribute("source", outputStr.c_str());

    tinyxml2::XMLElement* interpolationElement = a_doc->NewElement("input");
    samplerElement->InsertEndChild(interpolationElement);
    interpolationElement->SetAttribute("semantic", "INTERPOLATION");
    interpolationElement->SetAttribute("source", "#interpolation-source");
}

void AnimationSerializer::CreateColladaSource(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_index, const char* a_str, unsigned int a_count)
{
    const std::string idStr = std::string(a_id) + "-" + a_index + "-source";
    tinyxml2::XMLElement* sourceElement = a_doc->NewElement("source");
    a_parent->InsertEndChild(sourceElement);
    sourceElement->SetAttribute("id", idStr.c_str());

    const std::string arrStr = std::string(a_id) + "-" + a_index + "-array";
    tinyxml2::XMLElement* arrayElement  = a_doc->NewElement("float_array");
    sourceElement->InsertEndChild(arrayElement);
    arrayElement->SetAttribute("id", arrStr.c_str());
    arrayElement->SetAttribute("count", a_count);
    arrayElement->SetText(a_str);

    tinyxml2::XMLElement* techniqueElement = a_doc->NewElement("technique_common");
    sourceElement->InsertEndChild(techniqueElement);
    
    tinyxml2::XMLElement* accessorElement = a_doc->NewElement("accessor");
    techniqueElement->InsertEndChild(accessorElement);
    accessorElement->SetAttribute("source", ("#" + arrStr).c_str());
    accessorElement->SetAttribute("count", a_count);
    accessorElement->SetAttribute("stride", 1);

    tinyxml2::XMLElement* paramElement = a_doc->NewElement("param");
    accessorElement->InsertEndChild(paramElement);
    paramElement->SetAttribute("name", a_index);
    paramElement->SetAttribute("type", "float");
}
void AnimationSerializer::CreateColladaChannel(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_source, const char* a_object, const char* a_index)
{
    tinyxml2::XMLElement* channelElement = a_doc->NewElement("channel");
    a_parent->InsertEndChild(channelElement);

    const std::string sourceStr = std::string("#") + a_source + "-" + a_index + "-sampler";
    channelElement->SetAttribute("source", sourceStr.c_str());

    const std::string targetStr = std::string(a_object) + "/" + a_source + "." + a_index;
    channelElement->SetAttribute("target", targetStr.c_str());
}

void AnimationSerializer::WriteCollada(const Workspace* a_workspace, tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element, const Animation* a_animation)
{
    const char* name = a_animation->GetName();
    const std::list<AnimationGroup> groups = a_animation->GetNodes();

    for (auto iter = groups.begin(); iter != groups.end(); ++iter)
    {
        tinyxml2::XMLElement* rootAnimation = a_doc->NewElement("animation");
        a_element->InsertEndChild(rootAnimation);
        rootAnimation->SetAttribute("name", name);
        rootAnimation->SetAttribute("id", name);

        const Object* obj = a_workspace->GetObject(iter->ObjectID);
        if (obj != nullptr)
        {   
            tinyxml2::XMLElement* animationElement = a_doc->NewElement("animation");
            rootAnimation->InsertEndChild(animationElement);

            const char* objID = obj->GetIDName();

            animationElement->SetAttribute("name", name);
            const std::string idStr = std::string(objID) + "-" + name;
            animationElement->SetAttribute("id", idStr.c_str());

            CreateColladaSampler(a_doc, animationElement, "translate-X");
            CreateColladaSampler(a_doc, animationElement, "translate-Y");
            CreateColladaSampler(a_doc, animationElement, "translate-Z");

            CreateColladaSampler(a_doc, animationElement, "rotation-X");
            CreateColladaSampler(a_doc, animationElement, "rotation-Y");
            CreateColladaSampler(a_doc, animationElement, "rotation-Z");
            CreateColladaSampler(a_doc, animationElement, "rotation-ANGLE");

            CreateColladaSampler(a_doc, animationElement, "scale-X");
            CreateColladaSampler(a_doc, animationElement, "scale-Y");
            CreateColladaSampler(a_doc, animationElement, "scale-Z");
            
            const std::list<AnimationNode>& nodes = iter->Nodes;
            const unsigned int nodeCount = (unsigned int)nodes.size();

            std::string timeStr;
            std::string interpolationStr;

            std::string translationXStr;
            std::string translationYStr;
            std::string translationZStr;

            std::string rotationXStr;
            std::string rotationYStr;
            std::string rotationZStr;
            std::string rotationWStr;

            std::string scaleXStr;
            std::string scaleYStr;
            std::string scaleZStr;

            for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
            {
                timeStr += std::to_string(iter->Time) + " ";
                interpolationStr += "LINEAR ";

                const glm::vec3& trans = iter->Translation;
                const glm::quat& rot = iter->Rotation;
                const glm::vec4 axisAngle = glm::vec4(glm::axis(rot), glm::angle(rot));
                const glm::vec3& scale = iter->Scale;

                translationXStr += std::to_string(trans.x) + " ";
                translationYStr += std::to_string(trans.y) + " ";
                translationZStr += std::to_string(trans.z) + " ";

                rotationXStr += std::to_string(axisAngle.x) + " ";
                rotationYStr += std::to_string(axisAngle.y) + " ";
                rotationZStr += std::to_string(axisAngle.z) + " ";
                rotationWStr += std::to_string(axisAngle.w) + " ";

                scaleXStr += std::to_string(scale.x) + " ";
                scaleYStr += std::to_string(scale.y) + " ";
                scaleZStr += std::to_string(scale.z) + " ";
            }

            tinyxml2::XMLElement* interpolationSourceElement = a_doc->NewElement("source");
            animationElement->InsertEndChild(interpolationSourceElement);
            interpolationSourceElement->SetAttribute("id", "interpolation-source");

            tinyxml2::XMLElement* interpolationArrayElement = a_doc->NewElement("Name_array");
            interpolationSourceElement->InsertEndChild(interpolationArrayElement);
            interpolationSourceElement->SetAttribute("id", "interpolation-array");
            interpolationArrayElement->SetAttribute("count", nodeCount);
            interpolationArrayElement->SetText(interpolationStr.c_str());

            tinyxml2::XMLElement* interpolationTechniqueElement = a_doc->NewElement("technique_common");
            interpolationSourceElement->InsertEndChild(interpolationTechniqueElement);

            tinyxml2::XMLElement* interpolationAccessorElement = a_doc->NewElement("accessor");
            interpolationTechniqueElement->InsertEndChild(interpolationAccessorElement);
            interpolationAccessorElement->SetAttribute("source", "#interpolation-array");
            interpolationAccessorElement->SetAttribute("count", nodeCount);
            interpolationAccessorElement->SetAttribute("stride", 1);

            tinyxml2::XMLElement* interpolationParamElement = a_doc->NewElement("param");
            interpolationAccessorElement->InsertEndChild(interpolationParamElement);
            interpolationParamElement->SetAttribute("name", "INTERPOLATION");
            interpolationParamElement->SetAttribute("type", "Name");

            CreateColladaSource(a_doc, animationElement, "time", "TIME", timeStr.c_str(), nodeCount);

            CreateColladaSource(a_doc, animationElement, "translate", "X", translationXStr.c_str(), nodeCount);
            CreateColladaSource(a_doc, animationElement, "translate", "Y", translationYStr.c_str(), nodeCount);
            CreateColladaSource(a_doc, animationElement, "translate", "Z", translationZStr.c_str(), nodeCount);

            CreateColladaSource(a_doc, animationElement, "rotation", "X", rotationXStr.c_str(), nodeCount);
            CreateColladaSource(a_doc, animationElement, "rotation", "Y", rotationYStr.c_str(), nodeCount);
            CreateColladaSource(a_doc, animationElement, "rotation", "Z", rotationZStr.c_str(), nodeCount);
            CreateColladaSource(a_doc, animationElement, "rotation", "ANGLE", rotationWStr.c_str(), nodeCount);

            CreateColladaSource(a_doc, animationElement, "scale", "X", scaleXStr.c_str(), nodeCount);
            CreateColladaSource(a_doc, animationElement, "scale", "Y", scaleYStr.c_str(), nodeCount);
            CreateColladaSource(a_doc, animationElement, "scale", "Z", scaleZStr.c_str(), nodeCount);

            CreateColladaChannel(a_doc, animationElement, "translate", objID, "X");
            CreateColladaChannel(a_doc, animationElement, "translate", objID, "Y");
            CreateColladaChannel(a_doc, animationElement, "translate", objID, "Z");

            CreateColladaChannel(a_doc, animationElement, "rotation", objID, "X");
            CreateColladaChannel(a_doc, animationElement, "rotation", objID, "Y");
            CreateColladaChannel(a_doc, animationElement, "rotation", objID, "Z");
            CreateColladaChannel(a_doc, animationElement, "rotation", objID, "ANGLE");

            CreateColladaChannel(a_doc, animationElement, "scale", objID, "X");
            CreateColladaChannel(a_doc, animationElement, "scale", objID, "Y");
            CreateColladaChannel(a_doc, animationElement, "scale", objID, "Z");

            delete[] objID;
        }
    }
}

void AnimationSerializer::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element, const Animation* a_animation)
{
    tinyxml2::XMLElement* animationElement = a_doc->NewElement("Animation");
    a_element->InsertEndChild(animationElement);

    animationElement->SetAttribute("Name", a_animation->GetName());
    animationElement->SetAttribute("ReferenceFrameRate", a_animation->GetReferenceFramerate());
    animationElement->SetAttribute("Length", a_animation->GetAnimationLength());

    const std::list<AnimationGroup> groups = a_animation->GetNodes();
    for (auto iter = groups.begin(); iter != groups.end(); ++iter)
    {
        tinyxml2::XMLElement* groupElement = a_doc->NewElement("Group");
        animationElement->InsertEndChild(groupElement);

        groupElement->SetAttribute("ID", std::to_string(iter->ObjectID).c_str());

        const std::list<AnimationNode>& nodes = iter->Nodes;
        for (auto innerIter = nodes.begin(); innerIter != nodes.end(); ++innerIter)
        {
            tinyxml2::XMLElement* nodeElement = a_doc->NewElement("Node");
            groupElement->InsertEndChild(nodeElement);

            tinyxml2::XMLElement* timeElement = a_doc->NewElement("Time");
            nodeElement->InsertEndChild(timeElement);
            timeElement->SetText(innerIter->Time);

            XMLIO::WriteVec3(a_doc, nodeElement, "Translation", innerIter->Translation);
            XMLIO::WriteQuat(a_doc, nodeElement, "Rotation", innerIter->Rotation);
            XMLIO::WriteVec3(a_doc, nodeElement, "Scale", innerIter->Scale, glm::vec3(1.0f));
        }
    }
}
Animation* AnimationSerializer::ParseData(const tinyxml2::XMLElement* a_element)
{
    Animation* anim = nullptr;

    const char* name = a_element->Attribute("Name");
    if (name != nullptr)
    {
        anim = new Animation(name);

        anim->SetReferenceFramerate(a_element->IntAttribute("ReferenceFrameRate", 24));
        anim->SetAnimationLength(a_element->FloatAttribute("Length", 1.0f));

        for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
        {
            const char* str = iter->Value();
            if (strcmp(str, "Group") == 0)
            {
                long long id = std::stoll(iter->Attribute("ID"));

                for (const tinyxml2::XMLElement* innerIter = iter->FirstChildElement(); innerIter != nullptr; innerIter = innerIter->NextSiblingElement())
                {
                    const char* str = innerIter->Value();

                    if (strcmp(str, "Node") == 0)
                    {
                        AnimationNode n;
                        n.Time = 0.0f;
                        n.Translation = glm::vec3(0.0f);
                        n.Rotation = glm::identity<glm::quat>();
                        n.Scale = glm::vec3(1.0f);

                        for (const tinyxml2::XMLElement* kIter = innerIter->FirstChildElement(); kIter != nullptr; kIter = kIter->NextSiblingElement())
                        {
                            const char* str = kIter->Value();

                            if (strcmp(str, "Time") == 0)
                            {
                                n.Time = kIter->FloatText();
                            }
                            else if (strcmp(str, "Translation") == 0)
                            {
                                XMLIO::ReadVec3(kIter, &n.Translation);
                            }
                            else if (strcmp(str, "Rotation") == 0)
                            {
                                XMLIO::ReadQuat(kIter, &n.Rotation);
                            }
                            else if (strcmp(str, "Scale") == 0)
                            {
                                XMLIO::ReadVec3(kIter, &n.Scale);
                            }
                            else
                            {
                                printf("Animation::ParseData: Invalid Element: ");
                                printf(str);
                                printf("\n");
                            }
                        }

                        anim->AddNode(id, n);
                    }
                    else
                    {
                        printf("Animation::ParseData: Invalid Element: ");
                        printf(str);
                        printf("\n");
                    }
                }
            }
            else
            {
                printf("Animation::ParseData: Invalid Element: ");
                printf(str);
                printf("\n");
            }
        }
    }

    return anim;
}
void AnimationSerializer::PostParseData(Animation* a_animation, const std::unordered_map<long long, long long>& a_idMap)
{
    const std::list<AnimationGroup> groups = a_animation->GetNodes();

    a_animation->ClearNodes();

    for (auto iter = groups.begin(); iter != groups.end(); ++iter)
    {
        const auto idx = a_idMap.find(iter->ObjectID);
        if (idx != a_idMap.end())
        {
            const long long id = idx->second;

            for (auto innerIter = iter->Nodes.begin(); innerIter != iter->Nodes.end(); ++innerIter)
            {
                a_animation->AddNode(id, *innerIter);
            }
        }
    }
}