#include "Object.h"

#include <glad/glad.h>
#include <string>
#include <string.h>

#include "Camera.h"
#include "CurveModel.h"
#include "Datastore.h"
#include "Model.h"
#include "PathModel.h"
#include "Physics/CollisionObjects/Softbody.h"
#include "ShaderPixel.h"
#include "ShaderProgram.h"
#include "Shaders/AnimatorSoftbodyPixel.h"
#include "Shaders/AnimatorSoftbodyVertex.h"
#include "Shaders/AnimatorStandardPixel.h"
#include "Shaders/AnimatorStandardVertex.h"
#include "Shaders/EditorStandardPixel.h"
#include "Shaders/EditorStandardVertex.h"
#include "Shaders/ReferenceImagePixel.h"
#include "Shaders/ReferenceImageVertex.h"
#include "Shaders/WeightStandardPixel.h"
#include "Shaders/WeightStandardVertex.h"
#include "ShaderStorageBuffer.h"
#include "ShaderVertex.h"
#include "Texture.h"
#include "Transform.h"

long long Object::ObjectIDNum = 0;

Object::Object(const char* a_name, e_ObjectType a_objectType)
{
    m_visible = true;

    m_name = nullptr;

    m_transform = new Transform();
    m_animationTransform = new Transform();

    m_armatureBuffer = nullptr;

    m_parent = nullptr;

    m_objectType = a_objectType;

    SetName(a_name);

    m_curveModel = nullptr;
    m_pathModel = nullptr;

    m_collisionShape = nullptr;
    m_collisionObject = nullptr;

    m_referencePath = nullptr;
    m_referenceImage = nullptr;

    m_rootObject = nullptr;

    m_baseProgram = Datastore::GetShaderProgram("SHADER_EDITORSTANDARD");
    if (m_baseProgram == nullptr)
    {
        m_baseProgram = ShaderProgram::InitProgram("SHADER_EDITORSTANDARD", EDITORSTANDARDVERTEX, EDITORSTANDARDPIXEL);
    }

    m_animatorProgram = Datastore::GetShaderProgram("SHADER_ANIMATORSTANDARD");
    if (m_animatorProgram == nullptr)
    {
        m_animatorProgram = ShaderProgram::InitProgram("SHADER_ANIMATORSTANDARD", ANIMATORSTANDARDVERTEX, ANIMATORSTANDARDPIXEL);
    }
    m_animatorSBodyProgram = Datastore::GetShaderProgram("SHADER_ANIMATORSBODY");
    if (m_animatorSBodyProgram == nullptr)
    {
        m_animatorSBodyProgram = ShaderProgram::InitProgram("SHADER_ANIMATORSBODY", ANIMATORSOFTBODYVERTEX, ANIMATORSOFTBODYPIXEL);
    }

    m_weightProgram = Datastore::GetShaderProgram("SHADER_WEIGHTSTANDARD");
    if (m_weightProgram == nullptr)
    {
        m_weightProgram = ShaderProgram::InitProgram("SHADER_WEIGHTSTANDARD", WEIGHTSTANDARDVERTEX, WEIGHTSTANDARDPIXEL);
    }

    m_referenceProgram = Datastore::GetShaderProgram("SHADER_REFERENCEIMAGE");
    if (m_referenceProgram == nullptr)
    {
        m_referenceProgram = ShaderProgram::InitProgram("SHADER_REFERENCEIMAGE", REFERENCEIMAGEVERTEX, REFERENCEIMAGEPIXEL);
    }

    // Not the best but it works
    m_id = ObjectIDNum++;

    if (a_objectType == ObjectType_Armature)
    {
        Object* obj = new Object("Root", this);
        obj->SetParent(this);
    }
}
Object::Object(const char* a_name, Object* a_rootObject) :
    Object(a_name, ObjectType_ArmatureNode)
{
    m_rootObject = a_rootObject;
}
Object::~Object()
{
    if (m_curveModel != nullptr)
    {
        delete m_curveModel;
        m_curveModel = nullptr;
    }
    if (m_pathModel != nullptr)
    {
        delete m_pathModel;
        m_pathModel = nullptr;
    }

    if (m_collisionShape != nullptr)
    {
        delete m_collisionShape;
        m_collisionShape = nullptr;
    }
    if (m_collisionObject != nullptr)
    {
        delete m_collisionObject;
        m_collisionObject = nullptr;
    }

    if (m_name != nullptr)
    {
        delete[] m_name;
        m_name = nullptr;
    }

    if (m_armatureBuffer != nullptr)
    {
        delete m_armatureBuffer;
        m_armatureBuffer = nullptr;
    }

    delete m_transform;
    delete m_animationTransform;

    for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        delete *iter;
    }
}

bool Object::IsGlobalVisible() const
{
    if (m_parent != nullptr)
    {
        return m_visible && m_parent->IsGlobalVisible();
    }

    return m_visible;
}

char* Object::GetNameNoWhitespace() const
{
    const int len = strlen(m_name) + 1;
    char* name = new char[len];

    for (int i = 0; i < len; ++i)
    {
        if (m_name[i] == ' ')
        {
            name[i] = '-';

            continue;
        }

        name[i] = m_name[i];
    }

    return name;
}
char* Object::GetIDName() const
{
    const std::string str = "[" + std::to_string(m_id) + "]" + m_name;

    const int len = str.length() + 1;
    char* cStr = new char[len];

    for (int i = 0; i < len; ++i)
    {
        if (str[i] == ' ')
        {
            cStr[i] = '-';

            continue;
        }

        cStr[i] = str[i];
    }

    return cStr;
}
void Object::SetName(const char* a_name)
{   
    if (m_name != nullptr)
    {
        delete[] m_name;
        m_name = nullptr;
    }

    if (a_name != nullptr)
    {
        const int len = strlen(a_name);
        
        if (len != 0)
        {
            m_name = new char[len + 1];

            for (int i = 0; i <= len; ++i)
            {
                m_name[i] = a_name[i];
            }
        }
    }
}

void Object::SetParent(Object* a_parent)
{
    if (m_parent != nullptr)
    {
        for (auto iter = m_parent->m_children.begin(); iter != m_parent->m_children.end(); ++iter)
        {
            if ((*iter)->GetID() == m_id)
            {
                m_parent->m_children.erase(iter);

                break;
            }
        }

        m_parent = nullptr;
    }

    if (a_parent != nullptr)
    {
        m_parent = a_parent;
        m_parent->m_children.emplace_back(this);
    }
}

e_CollisionShapeType Object::GetCollisionShapeType() const
{
    if (m_collisionShape != nullptr)
    {
        return m_collisionShape->GetShapeType();
    }
    
    return CollisionShapeType_Null;
}

e_CollisionObjectType Object::GetCollisionObjectType() const
{
    if (m_collisionObject != nullptr)
    {
        return m_collisionObject->GetCollisionObjectType();
    }

    return CollisionObjectType_Null;
}

bool Object::SetReferenceImage(const char* a_path)
{
    if (a_path != nullptr)
    {
        const int size = strlen(a_path) + 1;

        m_referencePath = new char[size];

        for (int i = 0; i < size; ++i)
        {
            m_referencePath[i] = a_path[i];
        }

        m_referenceImage = Datastore::GetTexture(m_referencePath);

        return m_referenceImage != nullptr;
    }

    return false;
}

glm::mat4 Object::GetGlobalMatrix() const
{
    if (m_parent != nullptr)
    {
        return m_parent->GetGlobalMatrix() * m_transform->ToMatrix();
    }

    return m_transform->ToMatrix();
}
glm::mat4 Object::GetGlobalAnimMatrix() const
{
    if (m_parent != nullptr)
    {
        return m_parent->GetGlobalAnimMatrix() * m_animationTransform->ToMatrix();
    }

    return m_animationTransform->ToMatrix();
}
glm::vec3 Object::GetGlobalTranslation() const
{
    return GetGlobalMatrix()[3].xyz();
}
void Object::SetGlobalTranslation(const glm::vec3& a_pos)
{
    glm::mat4 inv = glm::mat4(1);
    if (m_parent != nullptr)
    {
        inv = glm::inverse(m_parent->GetGlobalMatrix());
    }

    const glm::vec4 pos = inv * glm::vec4(a_pos, 1);
    m_transform->Translation() = pos.xyz() / pos.w;
}

void Object::DrawModel(const Model* a_model, const glm::mat4& a_world, const glm::mat4& a_view, const glm::mat4& a_proj)
{
    const unsigned int programHandle = m_baseProgram->GetHandle();
    glUseProgram(programHandle);

    const unsigned int vao = a_model->GetVAO();
    glBindVertexArray(vao);

    glUniformMatrix4fv(0, 1, false, (float *)&a_view);
    glUniformMatrix4fv(1, 1, false, (float *)&a_proj);
    glUniformMatrix4fv(2, 1, false, (float *)&a_world);

    glDrawElements(GL_TRIANGLES, a_model->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Object::DrawBase(const Camera* a_camera, const glm::vec2& a_winSize)
{
    if (IsGlobalVisible())
    {
        const glm::mat4 view = a_camera->GetView();
        const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);

        const glm::mat4 world = GetGlobalMatrix();

        switch (m_objectType)
        {
        case ObjectType_ReferenceImage:
        {
            if (m_referenceImage != nullptr)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

                const unsigned int programHandle = m_referenceProgram->GetHandle();
                glUseProgram(programHandle);

                const unsigned int vao = Model::GetEmpty()->GetVAO();
                glBindVertexArray(vao);

                glUniformMatrix4fv(0, 1, false, (float*)&view);
                glUniformMatrix4fv(1, 1, false, (float*)&proj);
                glUniformMatrix4fv(2, 1, false, (float*)&world);

                const unsigned int texHandle = m_referenceImage->GetHandle();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texHandle);  
                glUniform1i(4, 0);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                glDisable(GL_BLEND);
            }

            break;
        }
        case ObjectType_CurveModel:
        {
            if (m_curveModel != nullptr)
            {
                const Model* model = m_curveModel->GetDisplayModel();
                if (model != nullptr)
                {   
                    DrawModel(model, world, view, proj);
                }
            }

            break;
        }
        case ObjectType_PathModel:
        {
            if (m_pathModel != nullptr)
            {
                const Model* model = m_pathModel->GetDisplayModel();
                if (model != nullptr)
                {
                    DrawModel(model, world, view, proj);
                }
            }

            break;
        }
        }
    }   
}

void UpdateMatrices(const glm::mat4& a_parent, const glm::mat4& a_animParent, const Object* a_obj, std::vector<glm::mat4>* a_matrices)
{
    if (a_obj != nullptr && a_obj->GetObjectType() == ObjectType_ArmatureNode)
    {
        const Transform* transform = a_obj->GetTransform();
        const Transform* animTransform = a_obj->GetAnimationTransform();

        const glm::mat4 matrix = transform->ToMatrix();
        const glm::mat4 global = a_parent * matrix;

        const glm::mat4 animMatrix = animTransform->ToMatrix();
        const glm::mat4 globalAnim = a_animParent * animMatrix;

        const glm::mat4 globalInv = glm::inverse(global);

        const glm::mat4 boneMat = globalInv * globalAnim;

        a_matrices->emplace_back(boneMat);

        const std::list<Object*> children = a_obj->GetChildren();
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            UpdateMatrices(global, globalAnim, *iter, a_matrices);
        }
    }
}

void Object::ResetAnimation()
{
    *m_animationTransform = *m_transform;

    if (m_collisionObject != nullptr)
    {
        m_collisionObject->Reset();
    }
}

e_AnimatorDrawMode Object::GetAnimatorDrawMode() const
{
    const Object* arm = nullptr;
    switch (m_objectType)
    {
    case ObjectType_CurveModel:
    {
        if (m_curveModel != nullptr)
        {
            arm = m_curveModel->GetArmature();
        }

        break;
    }
    case ObjectType_PathModel:
    {
        if (m_pathModel != nullptr)
        {
            arm = m_pathModel->GetArmature();
        }

        break;
    }
    }

    if (GetCollisionObjectType() == CollisionObjectType_Softbody)
    {
        if (arm != nullptr)
        {
            return AnimatorDrawMode_BoneSoftbody;
        }
        
        return AnimatorDrawMode_Softbody;
    }
    else if (arm != nullptr)
    {
        return AnimatorDrawMode_Bone;
    }

    return AnimatorDrawMode_Base;
}

void Object::DrawModelAnim(const Model* a_model, const Object* a_armature, unsigned int a_nodeCount, unsigned int a_armatureNodeCount, const glm::mat4& a_world, const glm::mat4& a_view, const glm::mat4& a_proj)
{
    const e_AnimatorDrawMode drawMode = GetAnimatorDrawMode();

    unsigned int programHandle = m_baseProgram->GetHandle();

    switch (drawMode)
    {
    case AnimatorDrawMode_Bone:
    {
        programHandle = m_animatorProgram->GetHandle();

        break;
    }
    case AnimatorDrawMode_Softbody:
    {   
        programHandle = m_animatorSBodyProgram->GetHandle();

        break;
    }
    }

    glUseProgram(programHandle);

    switch (drawMode)
    {
    case AnimatorDrawMode_Bone:
    {
        glUniform1ui(3, a_armatureNodeCount);

        const ShaderStorageBuffer* buffer = a_armature->m_armatureBuffer;
        if (buffer != nullptr)
        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buffer->GetHandle());
        }

        break;
    }
    case AnimatorDrawMode_Softbody:
    {
        if (m_collisionObject != nullptr)
        {
            Softbody* body = (Softbody*)m_collisionObject;
            body->UpdateDeltaStorageBuffer();
            
            glUniform1ui(3, a_nodeCount);

            const ShaderStorageBuffer* buffer = body->GetDeltaStorageBuffer();
            if (buffer != nullptr)
            {
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buffer->GetHandle());
            }
        }

        break;
    }
    }

    const unsigned int vao = a_model->GetVAO();
    glBindVertexArray(vao);

    glUniformMatrix4fv(0, 1, false, (float*)&a_view);
    glUniformMatrix4fv(1, 1, false, (float*)&a_proj);
    glUniformMatrix4fv(2, 1, false, (float*)&a_world);

    glDrawElements(GL_TRIANGLES, a_model->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Object::DrawAnimator(const Camera* a_camera, const glm::vec2& a_winSize)
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();

    if (IsGlobalVisible())
    {
        const glm::mat4 view = a_camera->GetView();
        const glm::mat4 proj = a_camera->GetProjection(a_winSize);

        const glm::mat4 world = GetGlobalMatrix();
        const glm::mat4 worldAnim = GetGlobalAnimMatrix();

        switch (m_objectType)
        {
        case ObjectType_Armature:
        {
            std::vector<glm::mat4> matrices;

            for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
            {
                UpdateMatrices(iden, iden, *iter, &matrices);
            }

            const unsigned int bufferSize = (unsigned int)(sizeof(glm::mat4) * matrices.size());

            if (m_armatureBuffer == nullptr)
            {
                m_armatureBuffer = new ShaderStorageBuffer(matrices.data(), bufferSize);
            }
            else
            {
                m_armatureBuffer->WriteData(matrices.data(), bufferSize);
            }

            break;
        }
        case ObjectType_ReferenceImage:
        {
            if (m_referenceImage != nullptr)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

                const unsigned int programHandle = m_referenceProgram->GetHandle();
                glUseProgram(programHandle);

                const unsigned int vao = Model::GetEmpty()->GetVAO();
                glBindVertexArray(vao);

                glUniformMatrix4fv(0, 1, false, (float*)&view);
                glUniformMatrix4fv(1, 1, false, (float*)&proj);
                glUniformMatrix4fv(2, 1, false, (float*)&world);

                const unsigned int texHandle = m_referenceImage->GetHandle();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texHandle);  
                glUniform1i(4, 0);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                glDisable(GL_BLEND);
            }

            break;
        }
        case ObjectType_CurveModel:
        {
            if (m_curveModel != nullptr)
            {
                const Model* model = m_curveModel->GetDisplayModel();
                if (model != nullptr)
                {
                    DrawModelAnim(model, m_curveModel->GetArmature(), m_curveModel->GetNodeCount(), m_curveModel->GetArmatureNodeCount(), worldAnim, view, proj);
                }                
            }

            break;
        }
        case ObjectType_PathModel:
        {
            if (m_pathModel != nullptr)
            {
                const Model* model = m_pathModel->GetDisplayModel();
                if (model != nullptr)
                {
                    DrawModelAnim(model, m_pathModel->GetArmature(), m_pathModel->GetPathNodeCount(), m_pathModel->GetArmatureNodeCount(), worldAnim, view, proj);
                }
            }

            break;
        }
        }
    }
}

void Object::DrawModelWeight(const Model* a_model, const Object* a_armature, unsigned int a_bone, unsigned int a_boneCount, const glm::mat4& a_world, const glm::mat4& a_view, const glm::mat4& a_proj)
{
    const unsigned int programHandle = m_weightProgram->GetHandle();
    glUseProgram(programHandle);

    const unsigned int vao = a_model->GetVAO();
    glBindVertexArray(vao);

    glUniformMatrix4fv(0, 1, false, (float *)&a_view);
    glUniformMatrix4fv(1, 1, false, (float *)&a_proj);
    glUniformMatrix4fv(2, 1, false, (float *)&a_world);
    glUniform1ui(5, a_boneCount);
    glUniform1ui(6, a_bone);

    glDrawElements(GL_TRIANGLES, a_model->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}
void Object::DrawWeight(const Camera* a_camera, const glm::vec2& a_winSize, unsigned int a_bone, unsigned int a_boneCount)
{
    if (IsGlobalVisible())
    {
        const glm::mat4 view = a_camera->GetView();
        const glm::mat4 proj = a_camera->GetProjection(a_winSize);

        const glm::mat4 world = GetGlobalMatrix();

        switch (m_objectType)
        {
        case ObjectType_CurveModel:
        {
            if (m_curveModel != nullptr)
            {
                const Model* model = m_curveModel->GetDisplayModel();
                const Object* arm = m_curveModel->GetArmature();

                if (arm != nullptr && model != nullptr)
                {
                    DrawModelWeight(model, arm, a_bone, a_boneCount, world, view, proj);
                }
            }

            break;
        }
        case ObjectType_PathModel:
        {
            if (m_pathModel != nullptr)
            {
                const Model* model = m_pathModel->GetDisplayModel();
                const Object* arm = m_pathModel->GetArmature();

                if (arm != nullptr && model != nullptr)
                {
                    DrawModelWeight(model, arm, a_bone, a_boneCount, world, view, proj);
                }
            }

            break;
        }
        }
    }
}