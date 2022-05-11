#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <list>
#include <vector>

#include "Physics/CollisionObjects/CollisionObject.h"
#include "Physics/CollisionShapes/CollisionShape.h"

class Animation; 
class ArmatureBody;
class Camera;
class CurveModel;
class Model;
class PathModel;
class PhysicsEngine;
class ShaderProgram;
class ShaderStorageBuffer;
class Texture;
class Transform;
class Workspace;

struct BoneGroup;

enum e_ObjectType
{
    ObjectType_Empty,
    ObjectType_CurveModel,
    ObjectType_PathModel,
    ObjectType_ReferenceImage,
    ObjectType_Armature,
    ObjectType_ArmatureNode
};

enum e_AnimatorDrawMode
{
    AnimatorDrawMode_Base,
    AnimatorDrawMode_Bone,
    AnimatorDrawMode_Softbody,
    AnimatorDrawMode_BoneSoftbody
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
    friend class ObjectSerializer;

    static long long ObjectIDNum;

    bool                 m_visible;
    
    long long            m_id;
            
    char*                m_name; 
            
    char*                m_referencePath;
    Texture*             m_referenceImage;
    
    e_ObjectType         m_objectType;
    
    Transform*           m_transform;
    Transform*           m_animationTransform;

    Object*              m_parent;
    std::list<Object*>   m_children;
    
    Object*              m_rootObject;

    ShaderStorageBuffer* m_armatureBuffer;
    ArmatureBody*        m_armatureBody;
    
    unsigned int         m_armatureMatrixCount;
    glm::mat4*           m_armatureMatrices;

    ShaderProgram*       m_baseProgram;

    ShaderProgram*       m_animatorProgram;
    ShaderProgram*       m_animatorSBodyProgram;
 
    ShaderProgram*       m_weightProgram;

    ShaderProgram*       m_referenceProgram;

    CurveModel*          m_curveModel;
    PathModel*           m_pathModel;

    CollisionShape*      m_collisionShape;
    CollisionObject*     m_collisionObject;

    void UpdateMatrices(const glm::mat4& a_parent, const glm::mat4& a_animParent, Object* a_obj, std::vector<glm::mat4>* a_matrices) const;
    void DrawModel(const Model* model, const glm::mat4& a_world, const glm::mat4& a_view, const glm::mat4& a_proj);
    void DrawModelAnim(const Model* a_model, const Object* a_armature, unsigned int a_nodeCount, unsigned int a_armatureNodeCount, const glm::mat4& a_world, const glm::mat4& a_view, const glm::mat4& a_proj);
    void DrawModelWeight(const Model* a_model, const Object* a_armature, unsigned int a_bone, unsigned int a_boneCount, const glm::mat4& a_world, const glm::mat4& a_view, const glm::mat4& a_proj);

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
    inline Transform* GetAnimationTransform() const
    {
        return m_animationTransform;
    }

    inline Object* GetRootObject() const
    {
        return m_rootObject;
    }

    ArmatureBody* GetArmatureBody(PhysicsEngine* a_engine);

    inline Object* GetParent() const
    {
        return m_parent;
    }
    void SetParent(Object* a_parent);

    inline std::list<Object*> GetChildren() const
    {
        return m_children;
    }

    inline unsigned int GetArmatureMatrixCount() const
    {
        return m_armatureMatrixCount;
    }
    inline glm::mat4* GetArmatureMatrices() const
    {
        return m_armatureMatrices;
    }

    inline e_ObjectType GetObjectType() const
    {
        return m_objectType;
    }

    inline PathModel* GetPathModel() const
    {
        return m_pathModel;
    }
    inline void SetPathModel(PathModel* a_pathModel)
    {
        m_pathModel = a_pathModel;
    }

    inline CurveModel* GetCurveModel() const
    {
        return m_curveModel;
    }
    inline void SetCurveModel(CurveModel* a_curveModel)
    {
        m_curveModel = a_curveModel;
    }

    inline CollisionShape* GetCollisionShape() const
    {
        return m_collisionShape;
    }
    inline void SetCollisionShape(CollisionShape* a_value)
    {
        m_collisionShape = a_value;
    }
    e_CollisionShapeType GetCollisionShapeType() const;

    inline CollisionObject* GetCollisionObject() const
    {
        return m_collisionObject;
    }
    inline void SetCollisionObject(CollisionObject* a_value)
    {
        m_collisionObject = a_value;
    }
    e_CollisionObjectType GetCollisionObjectType() const;

    bool SetReferenceImage(const char* a_path);

    glm::mat4 GetGlobalMatrix() const;
    glm::mat4 GetGlobalAnimMatrix() const;

    glm::vec3 GetGlobalTranslation() const;
    void SetGlobalTranslation(const glm::vec3& a_pos);

    void ResetAnimation();

    e_AnimatorDrawMode GetAnimatorDrawMode() const;

    void DrawBase(const Camera* a_camera, const glm::vec2& a_winSize);
    void DrawAnimator(const Camera* a_camera, const glm::vec2& a_winSize);
    void DrawWeight(const Camera* a_camera, const glm::vec2& a_winSize, unsigned int a_bone, unsigned int a_boneCount);
};