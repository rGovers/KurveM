#include "Animation.h"

#include <string.h>

#include "Object.h"
#include "Transform.h"

Animation::Animation(const char* a_name, Workspace* a_workspace)
{
    m_workspace = a_workspace;

    const int strLength = strlen(a_name) + 1;

    m_name = new char[strLength];

    for (int i = 0; i < strLength; ++i)
    {
        m_name[i] = a_name[i];
    }

    m_referenceFramerate = 24;
    m_length = 1.0f;
}
Animation::~Animation()
{
    delete[] m_name;
}

void Animation::AddNode(Object* a_object, const AnimationNode& a_node)
{
    for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
    {
        if (iter->SelectedObject == a_object)
        {
            for (auto innerIter = iter->Nodes.begin(); innerIter != iter->Nodes.end(); ++innerIter)
            {
                if (a_node.Time < innerIter->Time)
                {
                    iter->Nodes.emplace(innerIter, a_node);

                    return;
                }
            }

            iter->Nodes.emplace_back(a_node);

            return;
        }
    }

    AnimationGroup group;
    group.SelectedObject = a_object;
    group.Nodes.emplace_back(a_node);

    m_nodes.emplace_back(group);
}
void Animation::RemoveNode(Object* a_object, const AnimationNode& a_node)
{
    for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
    {
        if (iter->SelectedObject == a_object)
        {
            for (auto innerIter = iter->Nodes.begin(); innerIter != iter->Nodes.end(); ++innerIter)
            {
                if (a_node.Time == innerIter->Time)
                {
                    iter->Nodes.erase(innerIter);

                    break;
                }
            }

            if (iter->Nodes.size() == 0)
            {
                m_nodes.erase(iter);
            }

            return;
        }
    }
}

AnimationNode Animation::GetNode(Object* a_object, float a_time) const
{
    for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
    {
        if (iter->SelectedObject == a_object)
        {
            for (auto innerIter = iter->Nodes.begin(); innerIter != iter->Nodes.end(); ++innerIter)
            {
                if (innerIter->Time >= a_time)
                {
                    auto prev = innerIter;
                    --prev;

                    if (prev != iter->Nodes.end())
                    {
                        return *prev;
                    }

                    return *innerIter;
                }
            }

            break;
        }
    }

    Transform* transform = a_object->GetTransform();

    AnimationNode node;
    node.Time = -1;
    node.Translation = glm::vec3(0);
    node.Rotation = glm::identity<glm::quat>();
    node.Scale = glm::vec3(1);

    return node;
}
void Animation::SetNode(Object* a_object, const AnimationNode& a_node)
{
    for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
    {
        if (iter->SelectedObject == a_object)
        {
            for (auto innerIter = iter->Nodes.begin(); innerIter != iter->Nodes.end(); ++innerIter)
            {
                if (innerIter->Time == a_node.Time)
                {
                    innerIter->Translation = a_node.Translation;
                    innerIter->Rotation = a_node.Rotation;
                    innerIter->Scale = a_node.Scale;

                    return;
                }
            }

            return;
        }
    }
}

glm::vec3 Animation::GetTranslation(Object* a_object, float a_time) const
{
    Transform* transform = a_object->GetTransform();

    if (transform != nullptr)
    {
        for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
        {
            if (iter->SelectedObject == a_object)
            {
                for (auto innerIter = iter->Nodes.begin(); innerIter != iter->Nodes.end(); ++innerIter)
                {
                    float time = innerIter->Time;
                    glm::vec3 translation = innerIter->Translation;

                    if (time > m_length)
                    {
                        time = m_length;
                        translation = transform->Translation();
                    }

                    if (time > a_time)
                    {
                        auto prev = innerIter;
                        --prev;

                        glm::vec3 prevTranslation = transform->Translation();
                        float prevTime = 0.0f;

                        if (prev != iter->Nodes.end())
                        {
                            prevTranslation = prev->Translation;
                            prevTime = prev->Time;
                        }

                        const float timeDiff = time - prevTime;

                        const float lerp = (a_time - prevTime) / timeDiff;

                        return glm::mix(prevTranslation, translation, lerp);
                    }
                }

                break;
            }
        }

        return transform->Translation();
    }
    
    return glm::vec3(0);
}
glm::quat Animation::GetRotation(Object* a_object, float a_time) const
{
    Transform* transform = a_object->GetTransform();

    if (transform != nullptr)
    {
        for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
        {
            if (iter->SelectedObject == a_object)
            {
                for (auto innerIter = iter->Nodes.begin(); innerIter != iter->Nodes.end(); ++innerIter)
                {
                    float time = innerIter->Time;
                    glm::quat rotation = innerIter->Rotation;

                    if (time > m_length)
                    {
                        time = m_length;
                        rotation = transform->Quaternion();
                    }

                    if (time > a_time)
                    {
                        auto prev = innerIter;
                        --prev;

                        glm::quat prevRotation = transform->Quaternion();
                        float prevTime = 0.0f;

                        if (prev != iter->Nodes.end())
                        {
                            prevRotation = prev->Translation;
                            prevTime = prev->Time;
                        }

                        const float timeDiff = time - prevTime;

                        const float lerp = (a_time - prevTime) / timeDiff;

                        return glm::mix(prevRotation, rotation, lerp);
                    }
                }

                break;
            }
        }

        return transform->Quaternion();
    }
    
    return glm::identity<glm::quat>();
}
glm::vec3 Animation::GetScale(Object* a_object, float a_time) const
{
    Transform* transform = a_object->GetTransform();

    if (transform != nullptr)
    {
        for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
        {
            if (iter->SelectedObject == a_object)
            {
                for (auto innerIter = iter->Nodes.begin(); innerIter != iter->Nodes.end(); ++innerIter)
                {
                    float time = innerIter->Time;
                    glm::vec3 scale = innerIter->Scale;

                    if (time > m_length)
                    {
                        time = m_length;
                        scale = transform->Scale();
                    }

                    if (time > a_time)
                    {
                        auto prev = innerIter;
                        --prev;

                        glm::vec3 prevScale = transform->Scale();
                        float prevTime = 0.0f;

                        if (prev != iter->Nodes.end())
                        {
                            prevScale = prev->Translation;
                            prevTime = prev->Time;
                        }

                        const float timeDiff = time - prevTime;

                        const float lerp = (a_time - prevTime) / timeDiff;

                        return glm::mix(prevScale, scale, lerp);
                    }
                }

                break;
            }
        }

        return transform->Scale();
    }
    
    return glm::vec3(1);
}