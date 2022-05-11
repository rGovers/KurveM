#include "IO/AnimationSerializer.h"

#include <string>

#include "Animation.h"
#include "IO/XMLIO.h"
#include "Object.h"

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