#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <fstream>
#include <list>
#include <unordered_map>

#include "tinyxml2.h"

class Camera;
class CurveModel;
class Model;
class ShaderProgram;
class Texture;
class Transform;
class Workspace;

struct BoneGroup;

enum e_ObjectType
{
    ObjectType_Empty,
    ObjectType_CurveModel,
    ObjectType_ReferenceImage,
    ObjectType_Armature,
    ObjectType_ArmatureNode
};

struct ObjectBoneGroup
{
    long long ID;
    std::list<BoneGroup> Bones;
};

// Master Object class used for all objects in the heirarchy 
// Decided again polymorphism and component systems for easy access and just using an internal type to dictate usage
class Object
{
private:
    static long long ObjectIDNum;

    bool               m_visible;

    long long          m_id;
        
    char*              m_name; 
        
    char*              m_referencePath;
    Texture*           m_referenceImage;

    e_ObjectType       m_objectType;

    Transform*         m_transform;

    Object*            m_parent;
    std::list<Object*> m_children;

    Object*            m_rootObject;

    ShaderProgram*     m_program;
    ShaderProgram*     m_weightProgram;

    ShaderProgram*     m_referenceProgram;

    CurveModel*        m_curveModel;

protected:

public:
    Object(const char* a_name, e_ObjectType a_objectType = ObjectType_Empty);
    Object(const char* a_name, Object* a_rootObject);
    ~Object();

    bool IsGlobalVisible() const;
    inline bool IsVisible() const
    {
        return m_visible;
    }
    void SetVisible(bool a_value) 
    {
        m_visible = a_value;
    }

    inline const char* GetName() const
    {
        return m_name;
    }
    char* GetNameNoWhitespace() const;
    char* GetIDName() const;
    void SetName(const char* a_name);

    inline long long GetID() const
    {
        return m_id;
    }

    inline Transform* GetTransform() const
    {
        return m_transform;
    }

    inline Object* GetRootObject() const
    {
        return m_rootObject;
    }
    inline Object* GetParent() const
    {
        return m_parent;
    }
    void SetParent(Object* a_parent);

    inline std::list<Object*> GetChildren() const
    {
        return m_children;
    }

    inline e_ObjectType GetObjectType() const
    {
        return m_objectType;
    }

    inline CurveModel* GetCurveModel() const
    {
        return m_curveModel;
    }
    inline void SetCurveModel(CurveModel* a_curveModel)
    {
        m_curveModel = a_curveModel;
    }

    bool SetReferenceImage(const char* a_path);

    glm::mat4 GetGlobalMatrix() const;

    glm::vec3 GetGlobalTranslation() const;
    void SetGlobalTranslation(const glm::vec3& a_pos);

    void DrawBase(Camera* a_camera, const glm::vec2& a_winSize);
    void DrawWeight(Camera* a_camera, const glm::vec2& a_winSize, unsigned int a_bone, unsigned int a_boneCount);

    void WriteOBJ(std::ofstream* a_file, bool a_smartStep, int a_steps) const;
    tinyxml2::XMLElement* WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_geometryElement, tinyxml2::XMLElement* a_controllerElement, tinyxml2::XMLElement* a_parentElement, bool a_stepAdjust, int a_steps) const;
    void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element) const;

    static Object* ParseData(Workspace* a_workspace, const tinyxml2::XMLElement* a_element, Object* a_parent, std::list<ObjectBoneGroup>* a_boneGroups, std::unordered_map<long long, long long>* a_idMap);
    void PostParseData(const std::list<ObjectBoneGroup>& a_bones, const std::unordered_map<long long, long long>& a_idMap);
};