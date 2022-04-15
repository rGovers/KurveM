#include "Object.h"

#include <glad/glad.h>
#include <string>
#include <string.h>

#include "Camera.h"
#include "CurveModel.h"
#include "Datastore.h"
#include "Model.h"
#include "PathModel.h"
#include "ShaderPixel.h"
#include "ShaderProgram.h"
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
    const int len = strlen(m_name);
    char* name = new char[len];

    for (int i = 0; i <= len; ++i)
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

void Object::DrawBase(Camera* a_camera, const glm::vec2& a_winSize)
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

void UpdateMatrices(const glm::mat4& a_parent, const glm::mat4& a_animParent, Object* a_obj, std::vector<glm::mat4>* a_matrices)
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

void Object::DrawModelAnim(const Model* a_model, const Object* a_armature, const glm::mat4& a_world, const glm::mat4& a_view, const glm::mat4& a_proj)
{
    unsigned int programHandle = m_baseProgram->GetHandle();

    if (a_armature != nullptr)
    {
        programHandle = m_animatorProgram->GetHandle();
    }

    glUseProgram(programHandle);

    if (a_armature != nullptr)
    {
        const ShaderStorageBuffer* buffer = a_armature->m_armatureBuffer;
        const unsigned int bufferHandle = buffer->GetHandle();

        glUniform1ui(3, m_curveModel->GetArmatureNodeCount());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, bufferHandle);
    }

    const unsigned int vao = a_model->GetVAO();
    glBindVertexArray(vao);

    glUniformMatrix4fv(0, 1, false, (float*)&a_view);
    glUniformMatrix4fv(1, 1, false, (float*)&a_proj);
    glUniformMatrix4fv(2, 1, false, (float*)&a_world);

    glDrawElements(GL_TRIANGLES, a_model->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Object::DrawAnimator(Camera* a_camera, const glm::vec2& a_winSize)
{
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
                UpdateMatrices(glm::mat4(1), glm::mat4(1), *iter, &matrices);
            }

            if (m_armatureBuffer == nullptr)
            {
                m_armatureBuffer = new ShaderStorageBuffer(matrices.data(), matrices.size());
            }
            else
            {
                m_armatureBuffer->WriteData(matrices.data(), matrices.size());
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
                    DrawModelAnim(model, m_curveModel->GetArmature(), worldAnim, view, proj);
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
                    DrawModelAnim(model, nullptr, worldAnim, view, proj);
                }
            }

            break;
        }
        }
    }
}
void Object::DrawWeight(Camera* a_camera, const glm::vec2& a_winSize, unsigned int a_bone, unsigned int a_boneCount)
{
    if (IsGlobalVisible())
    {
        const glm::mat4 view = a_camera->GetView();
        const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);

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
                    const unsigned int programHandle = m_weightProgram->GetHandle();
                    glUseProgram(programHandle);

                    const unsigned int vao = model->GetVAO();
                    glBindVertexArray(vao);

                    glUniformMatrix4fv(0, 1, false, (float*)&view);
                    glUniformMatrix4fv(1, 1, false, (float*)&proj);
                    glUniformMatrix4fv(2, 1, false, (float*)&world);
                    glUniform1ui(5, a_boneCount);
                    glUniform1ui(6, a_bone);

                    glDrawElements(GL_TRIANGLES, model->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
                }
            }

            break;
        }
        }
    }
}

void Object::WriteOBJ(std::ofstream* a_file, bool a_smartStep, int a_steps, int a_pathSteps, int a_shapeSteps) const
{
    if (m_curveModel != nullptr)
    {
        a_file->write("o ", 2);
        a_file->write(m_name, strlen(m_name));
        
        const std::string str = std::to_string(m_id);
        a_file->write(str.c_str(), str.length());

        a_file->write("\n", 1);

        m_curveModel->WriteOBJ(a_file, a_smartStep, a_steps);
    }

    if (m_pathModel != nullptr)
    {
        a_file->write("o ", 2);
        a_file->write(m_name, strlen(m_name));

        const std::string str = std::to_string(m_id);
        a_file->write(str.c_str(), str.length());

        a_file->write("\n", 1);

        m_pathModel->WriteOBJ(a_file, a_pathSteps, a_shapeSteps);
    }
}
tinyxml2::XMLElement* GenerateColladaNodeElement(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_name, const char* a_type)
{
    tinyxml2::XMLElement* nodeElement = a_doc->NewElement("node");
    a_parent->InsertEndChild(nodeElement);
    nodeElement->SetAttribute("id", a_id);
    nodeElement->SetAttribute("name", a_name);
    nodeElement->SetAttribute("type", a_type);

    return nodeElement;
}
tinyxml2::XMLElement* GenerateColladaGeometryElement(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent, const char* a_id, const char* a_name)
{
    tinyxml2::XMLElement* geometryElement = a_doc->NewElement("geometry");
    a_parent->InsertEndChild(geometryElement);

    geometryElement->SetAttribute("id", a_id);
    geometryElement->SetAttribute("name", a_name);

    return geometryElement;
}
tinyxml2::XMLElement* Object::WriteCollada(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_geometryElement, tinyxml2::XMLElement* a_controllerElement, tinyxml2::XMLElement* a_parentElement, bool a_stepAdjust, int a_steps, int a_pathSteps, int a_shapeSteps) const
{
    const char* name = GetNameNoWhitespace();
    const char* idStr = GetIDName();

    tinyxml2::XMLElement* nodeElement = nullptr;

    switch (m_objectType)
    {
    case ObjectType_Armature:
    {
        nodeElement = GenerateColladaNodeElement(a_doc, a_parentElement, idStr, name, "NODE");

        break;
    }
    case ObjectType_ArmatureNode:
    {
        nodeElement = GenerateColladaNodeElement(a_doc, a_parentElement, idStr, name, "JOINT");

        break;
    }
    case ObjectType_CurveModel:
    {
        if (m_curveModel != nullptr)
        {
            tinyxml2::XMLElement* geometryElement = GenerateColladaGeometryElement(a_doc, a_geometryElement, idStr, name);
            nodeElement = GenerateColladaNodeElement(a_doc, a_parentElement, idStr, name, "NODE");

            tinyxml2::XMLElement* controllerElement = nullptr;
            const Object* armObject = m_curveModel->GetArmature();
            const std::string conStr = std::string(idStr) + "-controller"; 

            if (armObject != nullptr)
            {
                controllerElement = a_doc->NewElement("controller");

                controllerElement = a_doc->NewElement("controller");
                a_controllerElement->InsertEndChild(controllerElement);
                controllerElement->SetAttribute("id", conStr.c_str());
                controllerElement->SetAttribute("name", name);
            }

            char* outRoot;

            m_curveModel->WriteCollada(a_doc, geometryElement, controllerElement, idStr, name, a_stepAdjust, a_steps, &outRoot);

            if (armObject != nullptr)
            {
                tinyxml2::XMLElement* controllerInstanceElement = a_doc->NewElement("instance_controller");
                nodeElement->InsertEndChild(controllerInstanceElement);
                controllerInstanceElement->SetAttribute("url", ("#" + std::string(conStr)).c_str());
                
                tinyxml2::XMLElement* skeletonElement = a_doc->NewElement("skeleton");
                controllerInstanceElement->InsertEndChild(skeletonElement);
                skeletonElement->SetText(("#" + std::string(outRoot)).c_str());

                delete[] outRoot;
            }
            else
            {   
                tinyxml2::XMLElement* geometryInstanceElement = a_doc->NewElement("instance_geometry");
                nodeElement->InsertEndChild(geometryInstanceElement);
                geometryInstanceElement->SetAttribute("url", ("#" + std::string(idStr)).c_str());
            }
        }  

        break;
    }
    case ObjectType_PathModel:
    {
        if (m_pathModel != nullptr)
        {
            tinyxml2::XMLElement* geometryElement = GenerateColladaGeometryElement(a_doc, a_geometryElement, idStr, name);
            nodeElement = GenerateColladaNodeElement(a_doc, a_parentElement, idStr, name, "NODE");
            
            m_pathModel->WriteCollada(a_doc, geometryElement, idStr, name, a_pathSteps, a_shapeSteps);

            tinyxml2::XMLElement* geometryInstanceElement = a_doc->NewElement("instace_geomety");
            nodeElement->InsertEndChild(geometryInstanceElement);
            geometryInstanceElement->SetAttribute("url", ("#" + std::string(idStr)).c_str());
        }

        break;
    }
    }

    if (nodeElement != nullptr)
    {
        tinyxml2::XMLElement* translateElement = a_doc->NewElement("translate");
        nodeElement->InsertEndChild(translateElement);

        const glm::vec3 translation = m_transform->Translation();

        translateElement->SetText((std::to_string(translation.x) + " " + std::to_string(translation.y) + " " + std::to_string(translation.z)).c_str());

        tinyxml2::XMLElement* rotationElement = a_doc->NewElement("rotation");
        nodeElement->InsertEndChild(rotationElement);

        const glm::quat quaternion = m_transform->Quaternion();
        const glm::vec4 axisAngle = glm::vec4(glm::axis(quaternion), glm::degrees(glm::angle(quaternion)));

        rotationElement->SetText((std::to_string(axisAngle.x) + " " + std::to_string(axisAngle.y) + " " + std::to_string(axisAngle.z) + " " + std::to_string(axisAngle.w)).c_str());

        tinyxml2::XMLElement* scaleElement = a_doc->NewElement("scale");
        nodeElement->InsertEndChild(scaleElement);

        const glm::vec3 scale = m_transform->Scale();

        scaleElement->SetText((std::to_string(scale.x) + " " + std::to_string(scale.y) + " " + std::to_string(scale.z)).c_str());
    }

    delete[] name;
    delete[] idStr;

    return nodeElement;
}
void Object::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element) const
{
    a_element->SetAttribute("ID", std::to_string(m_id).c_str());
    a_element->SetAttribute("Name", m_name);   

    if (m_transform != nullptr)
    {
        m_transform->Serialize(a_doc, a_element);
    }

    if (m_referencePath != nullptr)
    {
        tinyxml2::XMLElement* element = a_doc->NewElement("ReferenceImage");
        a_element->InsertEndChild(element);

        element->SetAttribute("Path", m_referencePath);
    }

    if (m_curveModel != nullptr)
    {
        m_curveModel->Serialize(a_doc, a_element);
    }
    if (m_pathModel != nullptr)
    {
        m_pathModel->Serialize(a_doc, a_element);
    }
}

Object* Object::ParseData(Workspace* a_workspace, const tinyxml2::XMLElement* a_element, Object* a_parent, std::list<ObjectBoneGroup>* a_boneGroups, std::unordered_map<long long, long long>* a_idMap)
{
    const tinyxml2::XMLAttribute* idAtt = a_element->FindAttribute("ID");
    const tinyxml2::XMLAttribute* nameAtt = a_element->FindAttribute("Name");

    Object* obj = new Object(nameAtt->Value());
    if (a_parent != nullptr)
    {
        obj->SetParent(a_parent);
    }

    a_idMap->emplace(std::stoll(idAtt->Value()), obj->m_id);

    if (strcmp(a_element->Value(), "ArmatureNode") == 0)
    {
        obj->m_objectType = ObjectType_ArmatureNode;
    }

    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();

        if (strcmp(str, "Object") == 0)
        {
            ParseData(a_workspace, iter, obj, a_boneGroups, a_idMap);
        }
        else if (strcmp(str, "ArmatureNode") == 0)
        {
            obj->m_objectType = ObjectType_Armature;

            ParseData(a_workspace, iter, obj, a_boneGroups, a_idMap);
        }
        else if (strcmp(str, "Transform") == 0)
        {
            obj->m_transform->ParseData(iter);
        }
        else if (strcmp(str, "ReferenceImage") == 0)
        {
            obj->m_objectType = ObjectType_ReferenceImage;

            const char* val = iter->Attribute("Path");

            if (val != nullptr)
            {
                const int size = strlen(val) + 1;

                obj->m_referencePath = new char[size];

                for (int i = 0; i < size; ++i)
                {
                    obj->m_referencePath[i] = val[i];
                }

                obj->m_referenceImage = Datastore::GetTexture(obj->m_referencePath);
            }
        }
        else if (strcmp(str, "CurveModel") == 0)
        {
            std::list<BoneGroup> bones;

            obj->m_objectType = ObjectType_CurveModel;

            obj->m_curveModel = new CurveModel(a_workspace);
            obj->m_curveModel->ParseData(iter, &bones);

            if (bones.size() > 0)
            {
                ObjectBoneGroup boneGroup;
                boneGroup.ID = obj->m_id;
                boneGroup.Bones = bones;

                a_boneGroups->emplace_back(boneGroup);
            }
        }
        else if (strcmp(str, "PathModel") == 0)
        {
            obj->m_objectType = ObjectType_PathModel;

            obj->m_pathModel = new PathModel(a_workspace);
            obj->m_pathModel->ParseData(iter);
        }
        else
        {
            printf("Object::ParseData: Invalid Element: ");
            printf(str);
            printf("\n");
        }
    }

    return obj;
}
void Object::PostParseData(const std::list<ObjectBoneGroup>& a_bones, const std::unordered_map<long long, long long>& a_idMap)
{
    switch (m_objectType)
    {
    case ObjectType_CurveModel:
    {
        if (m_curveModel != nullptr)
        {
            for (auto iter = a_bones.begin(); iter != a_bones.end(); ++iter)
            {
                if (iter->ID == m_id)
                {
                    m_curveModel->PostParseData(iter->Bones, a_idMap);
                
                    break;
                }
            }

            m_curveModel->Triangulate();
        }

        break;
    }
    case ObjectType_PathModel:
    {
        if (m_pathModel != nullptr)
        {
            m_pathModel->Triangulate();
        }
    }
    }    
}