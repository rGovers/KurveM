#include "Editors/WeightPaintingEditor.h"

#include "Object.h"
#include "Workspace.h"

WeightPaintingEditor::WeightPaintingEditor(Editor* a_editor, Workspace* a_workspace)
{
    m_editor = a_editor;

    m_workspace = a_workspace;
}
WeightPaintingEditor::~WeightPaintingEditor()
{

}

e_EditorMode WeightPaintingEditor::GetEditorMode()
{
    return EditorMode_WeightPainting;
}

void WeightPaintingEditor::DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize)
{
    if (a_object == m_workspace->GetSelectedObject())
    {
        const e_ObjectType objectType = a_object->GetObjectType();

        unsigned int index = 0;
        unsigned int size = 0;

        switch (objectType)
        {
        case ObjectType_CurveModel:
        {
            const CurveModel* model = a_object->GetCurveModel();

            const std::list<Object*> nodes = model->GetArmatureNodes();
            size = nodes.size();

            const long long selectedID = m_editor->GetSelectedWeightNode();

            unsigned int iterIndex = 0;
            for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
            {
                const Object* obj = *iter;
                if (obj->GetID() == selectedID)
                {
                    index = iterIndex;

                    break;
                }

                ++iterIndex;
            }
        }
        }

        a_object->DrawWeight(a_camera, a_winSize, index, size);
    }
}

void WeightPaintingEditor::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize)
{

}
void WeightPaintingEditor::LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize)
{

}

void WeightPaintingEditor::Update(Camera* a_camera, const glm::vec2& a_screenSize, double a_delta)
{
    
}