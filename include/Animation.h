#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>
#include <list>

class Object;

struct AnimationNode
{
    float Time;

    glm::vec3 Translation;
    glm::quat Rotation;
    glm::vec3 Scale;
};

struct AnimationGroup
{
    long long ObjectID;

    std::list<AnimationNode> Nodes;
};

class Animation
{
private:       
    char*                     m_name;
          
    int                       m_referenceFramerate;
    float                     m_length;

    std::list<AnimationGroup> m_nodes;

protected:

public:
    Animation(const char* a_name);
    ~Animation();

    inline const char* GetName() const
    {
        return m_name;
    }

    inline int GetReferenceFramerate() const
    {
        return m_referenceFramerate;
    }
    void SetReferenceFramerate(int a_value)
    {
        m_referenceFramerate = a_value;
    }

    inline float GetAnimationLength() const
    {
        return m_length;
    }
    inline void SetAnimationLength(float a_value)
    {
        m_length = a_value;
    }

    inline std::list<AnimationGroup> GetNodes() const
    {
        return m_nodes;
    }

    void AddNode(const Object* a_object, const AnimationNode& a_node);
    void AddNode(long long a_object, const AnimationNode& a_node);

    void RemoveNode(const Object* a_object, const AnimationNode& a_node);
    void RemoveNode(long long a_object, const AnimationNode& a_node);

    void ClearNodes();

    AnimationNode GetNode(const Object* a_object, float a_time) const;
    AnimationNode GetKeyNode(const Object* a_object, int a_frame) const;

    void SetNode(const Object* a_object, const AnimationNode& a_node);

    bool ContainsObject(const Object* a_object) const;

    glm::vec3 GetTranslation(const Object* a_object, float a_time) const;
    glm::quat GetRotation(const Object* a_object, float a_time) const;
    glm::vec3 GetScale(const Object* a_object, float a_time) const;
};
